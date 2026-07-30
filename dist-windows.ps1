# Fabrique une archive distribuable de RespawnIRC pour Windows : l'exécutable rendu autonome par
# windeployqt (Qt et QtWebEngine copiés à côté de lui), accompagné des dossiers resources/ et
# themes/ que le programme lit à côté de lui, comme sous Linux et macOS. Ces deux dossiers ne sont
# jamais écrits : sous Windows tout ce que le programme écrit va dans userdata/, à côté de
# l'exécutable, ce qui garde l'ensemble portable.
#
# Usage : .\dist-windows.ps1 [-QtDir chemin\vers\Qt\5.15.2\msvc2019_64] [-Clean]
#         [-HunspellLibName hunspell-1.7] [-ZlibLibName zlibstatic]
# À défaut, le Qt utilisé est celui dont le qmake est dans le PATH.
#
# Les deux noms de bibliothèques ne servent qu'à ceux qui n'ont pas suivi la recette du README : les
# .pro prennent par défaut hunspell et, sous MSVC, zlib, qui sont les noms qu'elle produit. Le cas
# type est le hunspell-1.7 de vcpkg. -Clean recompile tout au lieu de reprendre l'existant.
#
# Le script trouve tout seul l'environnement MSVC avec vswhere, il n'y a donc pas besoin de le
# lancer depuis une invite de commandes développeur.
#
# La cible est Windows 10 64 bits ou plus récent. Le système fournit l'Universal CRT et
# D3Dcompiler_47.dll, il n'y a donc que deux choses à embarquer : OpenSSL, sans quoi aucune page
# n'est joignable, et les bibliothèques C++ de MSVC, sans lesquelles le programme ne démarre pas sur
# une machine où le redistribuable n'a jamais été installé (voir le README pour le détail).

[CmdletBinding()]
param(
    [string]$QtDir,
    [string]$HunspellLibName,
    [string]$ZlibLibName,
    [switch]$Clean
)

$ErrorActionPreference = 'Stop'

$repoDir = $PSScriptRoot
$distDir = Join-Path $repoDir 'dist'
$buildDir = Join-Path $repoDir 'build\respawnIrc'

# Résolution de Qt et vérification d'OpenSSL, partagées avec run-windows.ps1.
. (Join-Path $PSScriptRoot 'windows-common.ps1')

$qtDir = Resolve-QtDir -QtDir $QtDir
$qmakeBin = Join-Path $qtDir 'bin\qmake.exe'
$windeployqtBin = Join-Path $qtDir 'bin\windeployqt.exe'

foreach($thisBin in @($qmakeBin, $windeployqtBin))
{
    if(-not (Test-Path $thisBin))
    {
        throw "$thisBin est introuvable."
    }
}

# Charge les variables d'environnement de MSVC (cl, nmake, rc) dans la session courante :
# vcvars64.bat les pose dans son propre processus, on les récupère en lisant son `set` final.
# vswhere ignore les Build Tools sans -products *, ils ne sont pas considérés comme un produit.
function Import-MsvcEnvironment
{
    if(Get-Command nmake -ErrorAction SilentlyContinue)
    {
        return
    }

    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"

    if(-not (Test-Path $vswhere))
    {
        throw "vswhere introuvable : les Build Tools de Visual Studio ne sont pas installés."
    }

    $vsDir = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -format value -property installationPath

    if(-not $vsDir)
    {
        throw "Aucune installation MSVC avec les outils C++ x64 n'a été trouvée."
    }

    cmd /c "`"$vsDir\VC\Auxiliary\Build\vcvars64.bat`" > nul 2>&1 && set" | ForEach-Object {
        if($_ -match '^([^=]+)=(.*)$')
        {
            Set-Item -Path "env:$($matches[1])" -Value $matches[2]
        }
    }
}

# qmake, nmake et windeployqt écrivent leur progression sur la sortie d'erreur. Avec
# $ErrorActionPreference à Stop, PowerShell transforme chacune de ces lignes en erreur fatale alors
# que la commande a très bien fonctionné : on repasse donc en Continue le temps de l'appel, et on
# juge de la réussite sur le code de retour, qui est le seul indicateur fiable.
function Invoke-BuildTool
{
    param([Parameter(Mandatory)][scriptblock]$Command, [Parameter(Mandatory)][string]$Name)

    $previousPreference = $ErrorActionPreference
    $ErrorActionPreference = 'Continue'

    try
    {
        & $Command
    }
    finally
    {
        $ErrorActionPreference = $previousPreference
    }

    if($LASTEXITCODE -ne 0)
    {
        throw "$Name a échoué (code $LASTEXITCODE)."
    }
}

Import-MsvcEnvironment

# version.pri est la seule source du numéro de version, et le .pro le pousse de là dans le programme :
# l'archive et le binaire qu'elle contient ne peuvent donc pas annoncer deux numéros différents.
$sourceOfVersion = Get-Content (Join-Path $repoDir 'version.pri') -Raw

if($sourceOfVersion -notmatch '(?m)^\s*RESPAWNIRC_VERSION\s*=\s*([0-9.]+)\s*$')
{
    throw "Numéro de version introuvable dans version.pri."
}

$version = $matches[1]

Write-Host "== Compilation de RespawnIRC $version avec $qtDir"
# Compilation hors des sources : tout ce qui est produit reste dans build/, jamais dans respawnIrc/.
# Le dossier est celui de la compilation release à la main, décrite dans le README, et il est repris
# tel quel : fabriquer une archive après avoir essayé le programme ne recompile que ce qui a changé,
# là où l'effacement systématique d'avant coûtait les 45 sources à chaque fois. -Clean le retrouve.
if($Clean)
{
    Remove-Item -Recurse -Force $buildDir -ErrorAction SilentlyContinue
}

New-Item -ItemType Directory -Force -Path $buildDir | Out-Null

# En revanche l'exécutable en place est toujours effacé, et ce n'est pas un détail : le DESTDIR des
# .pro ne distingue ni release ni debug ni dossier de compilation, si bien que le RespawnIRC.exe de la
# racine peut venir d'ailleurs — d'un nmake debug, typiquement. nmake le comparerait alors à ses
# objets, le trouverait plus récent, n'éditerait aucun lien et l'archive emporterait ce binaire-là. Le
# retirer garantit que ce qui est empaqueté sort bien des objets de ce dossier-ci. C'est ce que
# l'effacement complet assurait avant, pour beaucoup plus cher.
Remove-Item (Join-Path $repoDir 'RespawnIRC.exe') -Force -ErrorAction SilentlyContinue

# Les noms de bibliothèques ne sont transmis que s'ils sont donnés : les valeurs par défaut des .pro
# sont déjà celles de la recette du README, et les répéter ici ne faisait rien. HUNSPELL_STATIC, lui,
# est nécessaire au Hunspell compilé à la main, dont les en-têtes déclareraient sinon tout en
# __declspec(dllimport) ; il est sans effet sur celui de vcpkg, qui engendre un hunvisapi.h au test
# déjà figé. Le passer toujours marche donc dans les deux cas.
$optionsForQmake = @('DEFINES+=HUNSPELL_STATIC')

if($HunspellLibName)
{
    $optionsForQmake += "HUNSPELL_LIB_NAME=$HunspellLibName"
}

if($ZlibLibName)
{
    $optionsForQmake += "ZLIB_LIB_NAME=$ZlibLibName"
}

Push-Location $buildDir

try
{
    Invoke-BuildTool -Name 'qmake' -Command {
        & $qmakeBin (Join-Path $repoDir 'respawnIrc\respawnIrc.pro') @optionsForQmake
    }

    Invoke-BuildTool -Name 'nmake' -Command { nmake release }
}
finally
{
    Pop-Location
}

Write-Host "== Assemblage du dossier distribuable"
# L'archive contient un unique dossier RespawnIRC, à décompresser tel quel : l'application et ses
# données doivent rester ensemble.
Remove-Item -Recurse -Force $distDir -ErrorAction SilentlyContinue
$imageDir = Join-Path $distDir 'image\RespawnIRC'
New-Item -ItemType Directory -Force -Path $imageDir | Out-Null

# Le DESTDIR de respawnIrc.pro produit l'exécutable à la racine du dépôt, pas dans build\.
Copy-Item (Join-Path $repoDir 'RespawnIRC.exe') $imageDir

Write-Host "== Copie de Qt à côté de l'exécutable"
# windeployqt copie les DLL de Qt, les greffons et QtWebEngineProcess.exe à côté de l'exécutable.
#
# --no-compiler-runtime lui évite d'embarquer vc_redist.x64.exe, 24 Mo que rien ne lance jamais et
# qui font doublon avec les DLL du runtime copiées plus bas. Il ne le copie que lorsque
# VCINSTALLDIR est définie, donc uniquement quand le script est lancé après vcvars64.bat, ce qui est
# toujours le cas ici : sans cet argument le gras dépend de la façon dont on appelle le script.
Invoke-BuildTool -Name 'windeployqt' -Command { & $windeployqtBin --release --no-compiler-runtime (Join-Path $imageDir 'RespawnIRC.exe') }

Write-Host "== Allègement"
# windeployqt copie les traductions de toutes les langues : le programme est en français, on ne
# garde que le français, plus l'anglais que Chromium utilise comme repli.
$localesDir = Join-Path $imageDir 'translations\qtwebengine_locales'
Get-ChildItem $localesDir -File | Where-Object { $_.Name -notin @('fr.pak', 'en-US.pak') } | Remove-Item -Force
Get-ChildItem (Join-Path $imageDir 'translations') -Filter 'qt_*.qm' -File |
    Where-Object { $_.Name -ne 'qt_fr.qm' } | Remove-Item -Force
# Les outils de développement de Chromium ne sont jamais ouverts depuis le programme.
Remove-Item (Join-Path $imageDir 'resources\qtwebengine_devtools_resources.pak') -Force -ErrorAction SilentlyContinue

# D3Dcompiler_47.dll, que windeployqt copie avec le lot ANGLE, fait partie du système depuis
# Windows 10 : le chargeur trouve celui de System32. Il n'était embarqué que pour Windows 7, où il
# manque généralement. Ne pas lire cette ligne comme s'il restait un rendu de secours dans l'archive :
# opengl32sw.dll est retiré juste en dessous, pour ses raisons propres, et plus rien ici ne rattrape un
# ANGLE en panne. Ce qui rattrape l'absence de pilote OpenGL est WARP, déjà dans le système et une
# couche plus bas — voir le commentaire suivant.
Remove-Item (Join-Path $imageDir 'D3Dcompiler_47.dll') -Force -ErrorAction SilentlyContinue

# opengl32sw.dll (20 Mo, le plus gros fichier retirable de l'archive) est le rendu OpenGL logiciel de
# Mesa. On a longtemps écrit ici qu'il était le seul recours des machines sans pilote OpenGL : c'est
# faux. Sans pilote du vendeur, l'OpenGL de bureau se limite au « GDI Generic » 1.1 du système,
# inutilisable pour Qt, mais le défaut de Qt bascule alors sur ANGLE, qui passe par Direct3D 11 et,
# faute de GPU, par WARP, le rasteriseur logiciel livré avec Windows. Le repli logiciel est donc déjà
# dans le système, une couche plus bas. Mesuré sur une machine virtuelle sans aucune accélération :
# GL_RENDERER vaut « ANGLE (Microsoft Basic Render Driver Direct3D11 vs_5_0 ps_5_0) » et QtWebEngine
# affiche correctement une page sans ce fichier. Les versions v3.1.6 à v3.1.10 publiées en amont ont
# d'ailleurs été distribuées ainsi, avec QtWebEngine et sans lui, pendant un an et demi.
# Il ne reste utile que si ANGLE lui-même échoue, ou si QT_OPENGL=software est forcé — ce dernier cas
# ne peut venir que d'une variable d'environnement posée à la main, jamais du programme.
Remove-Item (Join-Path $imageDir 'opengl32sw.dll') -Force -ErrorAction SilentlyContinue

Write-Host "== Bibliothèques d'exécution (cible Windows 10)"
# 1. OpenSSL, que Qt charge à l'exécution et sans lequel aucune page n'est joignable. Le détail est
#    dans windows-common.ps1, avec la vérification elle-même : ici son absence est une erreur franche,
#    une archive sans OpenSSL n'ayant aucun intérêt.
$opensslDir = Get-OpenSslDir -RepoDir $repoDir -Required

Copy-Item (Join-Path $opensslDir '*.dll') $imageDir -Force

# 2. Bibliothèques C++ de MSVC : absentes d'une machine où le redistribuable n'a jamais été
#    installé, quel que soit le Windows. C'est ce qui les distingue de l'Universal CRT abandonné
#    plus bas : sur un Windows 10 vierge, ucrtbase.dll est bien dans System32 alors que
#    msvcp140.dll et vcruntime140.dll n'y sont pas. Passer à Windows 10 ne les rend pas inutiles.
#
#    Tout le dossier est copié, sans liste de noms à tenir. Une liste figée de trois DLL a livré
#    pendant longtemps une archive qui ne démarrait pas du tout sur une machine vierge :
#    Qt5Core.dll et Qt5Widgets.dll importent aussi msvcp140_1.dll, et le chargeur s'arrête sur
#    « MSVCP140_1.dll est introuvable » avant la première ligne de code. Ce n'est pas une DLL que
#    ce dépôt choisit — elle est réclamée par les binaires précompilés de Qt 5.15.2, donc depuis
#    toujours et quel que soit le compilateur qui construit RespawnIRC. Relevé au dumpbin, quatre des
#    dix DLL du dossier sont réellement importées — msvcp140.dll, msvcp140_1.dll, vcruntime140.dll et
#    vcruntime140_1.dll — et msvcp140_1.dll était la seule des quatre à manquer ; les six autres ne
#    sont importées par rien. Les copier quand même coûte 1,1 Mo sur 158 et retire la question. Le
#    glob sur Microsoft.VC*.CRT évite au passage de figer le numéro de version des outils.
$crtDir = Get-ChildItem (Join-Path $env:VCToolsRedistDir 'x64') -Directory -Filter 'Microsoft.VC*.CRT' -ErrorAction SilentlyContinue |
    Select-Object -First 1

if(-not $crtDir)
{
    throw "Bibliothèques d'exécution MSVC introuvables sous $env:VCToolsRedistDir."
}

Copy-Item (Join-Path $crtDir.FullName '*.dll') $imageDir -Force

# L'Universal CRT n'est pas copié : ucrtbase.dll et les api-ms-win-* sont des composants du système
# depuis Windows 10, et les seconds n'y sont même pas des fichiers, le chargeur résolvant ces noms
# par le schéma d'API sets du noyau. La quarantaine de DLL que le dépôt distribuait n'existait que
# pour Windows 7, où l'Universal CRT n'arrivait que par la mise à jour facultative KB2999226.

Write-Host "== Vérification des dépendances"
# Une archive incomplète ne se voit pas sur la machine qui la fabrique : installer les Build Tools
# pose msvcp140.dll et toute sa famille dans System32, et le chargeur les y trouve. C'est ce qui a
# laissé passer l'absence de msvcp140_1.dll, et c'est la deuxième fois qu'une archive silencieusement
# incomplète est sortie d'ici. On vérifie donc que chaque DLL du runtime C++ réclamée par un binaire
# de l'archive est bien dans l'archive, plutôt que de s'en remettre à la machine de compilation.
#
# Portée volontairement étroite : les imports statiques de la famille du runtime MSVC, les seuls que
# ni Windows ni windeployqt ne fournissent. Le reste des imports est soit dans l'archive, soit fourni
# par le système ; OpenSSL n'apparaît pas ici puisque Qt le charge dynamiquement, et il a déjà sa
# propre vérification plus haut. Un import chargé à la main par LoadLibrary échapperait aussi à ce
# contrôle : il ne remplace pas un essai sur une machine sans redistribuable Visual C++.
if(-not (Get-Command dumpbin -ErrorAction SilentlyContinue))
{
    throw "dumpbin est introuvable alors que l'environnement MSVC est chargé : la vérification des dépendances ne peut pas se faire, et la sauter rendrait le contrôle inutile."
}

$namesInImage = @{}
Get-ChildItem $imageDir -Recurse -File -Include '*.dll', '*.exe' |
    ForEach-Object { $namesInImage[$_.Name.ToLower()] = $true }

$missingRuntime = @{}
$runtimeImportsSeen = 0
$previousPreference = $ErrorActionPreference
$ErrorActionPreference = 'Continue'

try
{
    foreach($thisPe in (Get-ChildItem $imageDir -Recurse -File -Include '*.dll', '*.exe'))
    {
        foreach($thisLine in (& dumpbin /nologo /dependents $thisPe.FullName 2>&1))
        {
            # Les lignes d'imports de dumpbin sont indentées de quatre espaces et ne portent qu'un nom.
            if($thisLine -match '^\s{4}(\S+\.dll)\s*$')
            {
                $thisImport = $matches[1].ToLower()

                if($thisImport -match '^(msvcp|vcruntime|concrt|vccorlib)\d')
                {
                    $runtimeImportsSeen++

                    if(-not $namesInImage.ContainsKey($thisImport))
                    {
                        $missingRuntime[$thisImport] = $true
                    }
                }
            }
        }
    }
}
finally
{
    $ErrorActionPreference = $previousPreference
}

# Ce contrôle-ci vise le contrôle lui-même, et il n'est pas décoratif : un relevé qui ne trouve rien
# conclurait que tout va bien. Or l'archive importe forcément le runtime C++, des dizaines de binaires
# de Qt le réclamant — 98 imports relevés en juillet 2026, chiffre donné pour situer l'ordre de
# grandeur et sur lequel le test ne s'appuie pas : seul zéro est traité comme impossible. Zéro ne peut
# vouloir dire que la sortie de dumpbin a changé de forme, ou que l'expression rationnelle ci-dessus ne
# mord plus, jamais que l'archive est saine.
if($runtimeImportsSeen -eq 0)
{
    throw "Le relevé des dépendances n'a trouvé aucun import du runtime C++, ce qui est impossible : c'est la vérification qui est cassée, pas l'archive qui est propre. Comparer la sortie de dumpbin /dependents à l'expression rationnelle qui la lit."
}

if($missingRuntime.Count -gt 0)
{
    throw "L'archive serait incomplète : $(($missingRuntime.Keys | Sort-Object) -join ', ') réclamée(s) par ses binaires mais absente(s). Ces DLL ne font partie d'aucun Windows, le programme ne démarrerait pas sur une machine où le redistribuable Visual C++ n'a jamais été installé."
}

Write-Host "   ok, $runtimeImportsSeen imports du runtime C++ relevés, aucun manquant"

Write-Host "== Données du programme"
# resources/ et themes/ sont extraits de git et non copiés depuis le dossier de travail : celui-ci
# contient aussi ce que le mainteneur a accumulé en se servant du programme, à commencer par les
# stickers qu'une version antérieure téléchargeait dans resources/stickers/ et que rien ne distingue
# de ceux livrés. git archive ne sort que ce qui est commité, sans liste d'exclusion à tenir à jour.
# Ce que le programme écrit aujourd'hui vit dans userdata/, qui n'est simplement jamais copié.
if(-not (Get-Command git -ErrorAction SilentlyContinue))
{
    throw "git est introuvable : il sert à extraire resources\ et themes\ sans y mêler de données personnelles."
}

$archiveOfData = Join-Path $distDir 'donnees.zip'

Invoke-BuildTool -Name 'git archive' -Command {
    & git -C $repoDir archive --format=zip --output=$archiveOfData HEAD resources themes
}

# windeployqt a déjà créé un dossier resources/ à côté de l'exécutable pour QtWebEngine (icudtl.dat
# et les fichiers .pak). C'est le même nom que celui des données du programme, qui doivent elles
# aussi être à côté de l'exécutable : les deux contenus cohabitent donc dans un seul dossier, ce que
# rien n'empêche puisque aucun nom de fichier ne se chevauche. Il faut fusionner et non remplacer,
# ce que fait Expand-Archive en écrivant dans un dossier déjà peuplé.
Expand-Archive -Path $archiveOfData -DestinationPath $imageDir -Force
Remove-Item $archiveOfData -Force

$zipPath = Join-Path $distDir "RespawnIRC-$version-windows.zip"

# Compress-Archive était de loin l'étape la plus lente du script sur ces 159 Mo et ces 426 fichiers.
# CreateFromDirectory fait la même archive nettement plus vite. Le dernier argument est
# includeBaseDirectory : à $true, les entrées commencent par RespawnIRC\, ce qui donne bien le dossier
# unique à décompresser tel quel, comme le -Path sur le dossier le faisait avant.
Add-Type -AssemblyName System.IO.Compression.FileSystem
[System.IO.Compression.ZipFile]::CreateFromDirectory($imageDir, $zipPath, [System.IO.Compression.CompressionLevel]::Optimal, $true)
Remove-Item -Recurse -Force (Join-Path $distDir 'image')

Write-Host "== Terminé : $zipPath"
