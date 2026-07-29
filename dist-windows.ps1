# Fabrique une archive distribuable de RespawnIRC pour Windows : l'exécutable rendu autonome par
# windeployqt (Qt et QtWebEngine copiés à côté de lui), accompagné des dossiers resources/ et
# themes/ que le programme lit à côté de lui, comme sous Linux et macOS. Ces deux dossiers ne sont
# jamais écrits : sous Windows tout ce que le programme écrit va dans userdata/, à côté de
# l'exécutable, ce qui garde l'ensemble portable.
#
# Usage : .\dist-windows.ps1 [-QtDir chemin\vers\Qt\5.15.2\msvc2019_64]
# À défaut, le Qt utilisé est celui dont le qmake est dans le PATH.
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
    [string]$HunspellLibName = 'hunspell',
    [string]$ZlibLibName = 'zs'
)

$ErrorActionPreference = 'Stop'

$repoDir = $PSScriptRoot
$distDir = Join-Path $repoDir 'dist'
$buildDir = Join-Path $repoDir 'build\respawnIrc'

if($QtDir)
{
    $qtDir = $QtDir
}
elseif(Get-Command qmake -ErrorAction SilentlyContinue)
{
    $qtDir = Split-Path (Split-Path (Get-Command qmake).Source)
}
else
{
    throw "Qt introuvable : passez le chemin de Qt avec -QtDir, ou mettez son qmake dans le PATH."
}

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

$sourceOfVersion = Get-Content (Join-Path $repoDir 'respawnIrc\respawnIrc.cpp') -Raw

if($sourceOfVersion -notmatch 'currentVersionName\("v([0-9.]+)"\)')
{
    throw "Numéro de version introuvable dans respawnIrc.cpp."
}

$version = $matches[1]

Write-Host "== Compilation de RespawnIRC $version avec $qtDir"
# Compilation hors des sources : tout ce qui est produit reste dans build/, jamais dans respawnIrc/.
Remove-Item -Recurse -Force $buildDir -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Force -Path $buildDir | Out-Null
Push-Location $buildDir

try
{
    # HUNSPELL_STATIC est nécessaire au Hunspell compilé à la main, dont les en-têtes déclareraient
    # sinon tout en __declspec(dllimport) ; il est sans effet sur celui de vcpkg, qui engendre un
    # hunvisapi.h au test déjà figé. Le passer toujours marche donc dans les deux cas.
    Invoke-BuildTool -Name 'qmake' -Command {
        & $qmakeBin (Join-Path $repoDir 'respawnIrc\respawnIrc.pro') "HUNSPELL_LIB_NAME=$HunspellLibName" "ZLIB_LIB_NAME=$ZlibLibName" 'DEFINES+=HUNSPELL_STATIC'
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
# qui font doublon avec les trois DLL du runtime copiées plus bas. Il ne le copie que lorsque
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
# manque généralement. Si ANGLE échouait malgré tout, le repli reste opengl32sw.dll ci-dessous.
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
# 1. OpenSSL : Qt 5.15.2 charge libssl-1_1/libcrypto-1_1 à l'exécution pour tout ce qui est HTTPS.
#    Sans elles QSslSocket::supportsSsl() est faux et aucune page de jeuxvideo.com n'est joignable.
#    windeployqt ne les copie pas et Qt ne les fournit plus (voir le README).
$opensslDir = Join-Path $repoDir 'openssl\bin'

if(-not (Test-Path (Join-Path $opensslDir 'libssl-1_1-x64.dll')))
{
    throw "OpenSSL introuvable dans openssl\bin : sans lui le programme ne peut joindre aucune page (voir le README)."
}

Copy-Item (Join-Path $opensslDir '*.dll') $imageDir -Force

# 2. Bibliothèques C++ de MSVC : absentes d'une machine où le redistribuable n'a jamais été
#    installé, quel que soit le Windows. C'est ce qui les distingue de l'Universal CRT abandonné
#    plus bas : sur un Windows 10 vierge, ucrtbase.dll est bien dans System32 alors que
#    msvcp140.dll et vcruntime140.dll n'y sont pas. Passer à Windows 10 ne les rend pas inutiles.
$crtDir = Get-ChildItem (Join-Path $env:VCToolsRedistDir 'x64\Microsoft.VC143.CRT') -ErrorAction SilentlyContinue

if(-not $crtDir)
{
    throw "Bibliothèques d'exécution MSVC introuvables sous $env:VCToolsRedistDir."
}

foreach($thisDll in @('vcruntime140.dll', 'vcruntime140_1.dll', 'msvcp140.dll'))
{
    Copy-Item (Join-Path $env:VCToolsRedistDir "x64\Microsoft.VC143.CRT\$thisDll") $imageDir -Force
}

# L'Universal CRT n'est pas copié : ucrtbase.dll et les api-ms-win-* sont des composants du système
# depuis Windows 10, et les seconds n'y sont même pas des fichiers, le chargeur résolvant ces noms
# par le schéma d'API sets du noyau. La quarantaine de DLL que le dépôt distribuait n'existait que
# pour Windows 7, où l'Universal CRT n'arrivait que par la mise à jour facultative KB2999226.

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
Compress-Archive -Path (Join-Path $distDir 'image\RespawnIRC') -DestinationPath $zipPath
Remove-Item -Recurse -Force (Join-Path $distDir 'image')

Write-Host "== Terminé : $zipPath"
