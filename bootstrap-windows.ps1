# Installe de quoi compiler RespawnIRC sous Windows sur une machine vierge : les Build Tools de
# Visual Studio, Qt 5.15.2 avec QtWebEngine, Hunspell et zlib compilés à la main, et OpenSSL 1.1.1.
# Tout est posé dans la disposition attendue par les .pro, il n'y a rien à déplacer ensuite.
#
# Usage, depuis un PowerShell administrateur, à la racine du dépôt :
#     powershell -ExecutionPolicy Bypass -File .\bootstrap-windows.ps1
#         [-QtRootDir C:\Qt] [-SkipBuildTools] [-SkipQt] [-KeepDownloads]
#
# Le -ExecutionPolicy Bypass est nécessaire et pas décoratif : un Windows 10 neuf est en Restricted
# et refuse le script avant de l'avoir lu. Si le dépôt vient d'une archive zip et non d'un git clone,
# il faut en plus un Unblock-File, la marque de provenance de Windows bloquant le script même ainsi.
#
# Un PowerShell ordinaire suffit : seule l'installation des Build Tools a besoin des droits
# d'administrateur, et le script élève cet installateur-là par une invite UAC plutôt que de réclamer
# d'être lancé élevé. L'invite n'apparaît que si les Build Tools manquent vraiment — sur une machine
# déjà équipée, l'étape se saute sans rien demander. Un PowerShell administrateur reste accepté, et
# ne fait alors apparaître aucune invite.
#
# Le script est réentrant : chaque étape est sautée si son résultat est déjà là, on peut donc le
# relancer après un échec sans tout retélécharger.
#
# Compter une trentaine de minutes et environ 4,5 Go sur le disque, presque entièrement pour les
# Build Tools (3,3 Go) et Qt (0,9 Go mesuré, QtWebEngine compris), le reste étant négligeable :
# Hunspell et zlib pèsent 2,4 Mo de téléchargement et se compilent en une quinzaine de secondes.
#
# Ce script est en UTF-8 avec BOM, comme dist-windows.ps1 et pour la même raison : PowerShell 5.1 lit
# un .ps1 comme de l'ANSI sans lui et tous les accents des messages sont abîmés.

[CmdletBinding()]
param(
    [string]$QtRootDir = 'C:\Qt',
    [string]$QtVersion = '5.15.2',
    [string]$QtArch = 'win64_msvc2019_64',
    # Le numéro du SDK est à adapter, c'est celui qui était courant quand ces lignes ont été écrites.
    [string]$WindowsSdkComponent = 'Microsoft.VisualStudio.Component.Windows11SDK.26100',
    [string]$AqtVersion = 'v3.3.0',
    [string]$HunspellVersion = '1.7.3',
    [string]$ZlibVersion = '1.3.1',
    [string]$OpenSslVersion = '1.1.1w',
    # Empreinte publiée par FireDaemon. La 1.1.1w étant la dernière version de la branche 1.1.1, ce
    # fichier ne bouge plus ; si la vérification échoue, ne pas la contourner sans comprendre.
    [string]$OpenSslSha256 = '1870B15BF6749E65FFBBADF52CDFF3EE0E9F02943550BF4395574BB432AF3EB8',
    [switch]$SkipBuildTools,
    [switch]$SkipQt,
    [switch]$KeepDownloads
)

$ErrorActionPreference = 'Stop'
$ProgressPreference = 'SilentlyContinue'
[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12

$repoDir = $PSScriptRoot
$downloadDir = Join-Path $repoDir 'build\bootstrap'
$qtDir = Join-Path $QtRootDir "$QtVersion\msvc2019_64"

# Les mêmes que dans dist-windows.ps1, recopiées pour que ce script reste autonome : il tourne avant
# que quoi que ce soit d'autre n'existe sur la machine. Voir POSSIBLE-BUILD-SIMPLIFICATIONS.md,
# point 4, pour le windows-common.ps1 qui les réunirait un jour.
function Import-MsvcEnvironment
{
    if(Get-Command nmake -ErrorAction SilentlyContinue)
    {
        return
    }

    # vswhere ignore les Build Tools sans -products *, ils ne sont pas considérés comme un produit.
    $vswhereBin = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"

    if(-not (Test-Path $vswhereBin))
    {
        throw "vswhere est introuvable : les Build Tools de Visual Studio ne sont pas installés."
    }

    $vsDir = & $vswhereBin -products * -latest -property installationPath

    if(-not $vsDir)
    {
        throw "Aucune installation de Visual Studio trouvée par vswhere."
    }

    $vcvarsBin = Join-Path $vsDir 'VC\Auxiliary\Build\vcvars64.bat'

    if(-not (Test-Path $vcvarsBin))
    {
        throw "$vcvarsBin est introuvable : le composant VC.Tools.x86.x64 manque à l'installation."
    }

    # vcvars64.bat pose ses variables dans son propre processus, on les récupère en lisant son `set`.
    # Il écrit au passage une ligne « 'vswhere.exe' is not recognized » sur sa sortie d'erreur, qui
    # arrive donc directement à la console : elle vient de son propre code, elle est sans effet, et
    # dist-windows.ps1 la produit à l'identique. Ne pas la faire taire avec un 2>$null, ce qui
    # masquerait aussi les vraies erreurs de vcvars.
    & cmd /c "`"$vcvarsBin`" && set" | ForEach-Object {
        if($_ -match '^([^=]+)=(.*)$')
        {
            Set-Item -Path "Env:$($matches[1])" -Value $matches[2]
        }
    }
}

# Les outils natifs écrivent leur progression sur la sortie d'erreur : avec $ErrorActionPreference à
# 'Stop', chaque ligne deviendrait une erreur fatale alors que la commande a réussi. On juge donc sur
# le code de retour, et sur lui seul.
function Invoke-BuildTool
{
    param([string]$Name, [scriptblock]$Command)

    $previousPreference = $ErrorActionPreference
    $ErrorActionPreference = 'Continue'

    try
    {
        & $Command 2>&1 | ForEach-Object { "$_" } | Out-Null
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

function Get-FileIfNeeded
{
    param([string]$Url, [string]$Path)

    if(Test-Path $Path)
    {
        Write-Host "   déjà téléchargé : $(Split-Path $Path -Leaf)"
        return
    }

    Write-Host "   téléchargement de $(Split-Path $Path -Leaf)..."
    Invoke-WebRequest $Url -OutFile $Path -TimeoutSec 1800
}

New-Item -ItemType Directory -Force -Path $downloadDir | Out-Null

# 1. Les Build Tools. Les deux composants sont nécessaires : VC.Tools.x86.x64 seul pose bien cl.exe
#    mais aucun Windows Kits, et depuis Visual Studio 2015 les en-têtes de la bibliothèque C standard
#    appartiennent au SDK et pas au compilateur — un #include <stdio.h> suffit à s'en rendre compte.
Write-Host "== 1/5 Build Tools de Visual Studio"

if($SkipBuildTools)
{
    Write-Host "   ignoré (-SkipBuildTools)"
}
elseif(Test-Path "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe")
{
    Write-Host "   déjà installés"
}
else
{
    # Seule cette étape demande l'élévation : les quatre autres écrivent dans le dépôt et dans
    # $QtRootDir. On ne la réclame donc qu'ici, et pas en tête de script, pour qu'une reprise après
    # coup ou un -SkipBuildTools puisse tourner depuis un PowerShell ordinaire. C'est aussi ce qui
    # fait qu'aucune invite UAC n'apparaît sur une machine déjà équipée : le `elseif` ci-dessus a
    # rendu la main avant d'arriver ici.
    $identity = [Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()
    $isAdmin = $identity.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)

    $buildToolsBin = Join-Path $downloadDir 'vs_BuildTools.exe'
    Get-FileIfNeeded -Url 'https://aka.ms/vs/17/release/vs_BuildTools.exe' -Path $buildToolsBin

    Write-Host "   installation (3,3 Go, une quinzaine de minutes, sans interface)..."

    $installArgs = @(
        '--quiet', '--wait', '--norestart',
        '--add', 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64',
        '--add', $WindowsSdkComponent
    )

    # Plutôt que d'exiger un PowerShell déjà élevé, on n'élève que l'installateur, par le -Verb RunAs
    # qui déclenche l'invite UAC. Le reste du script continue dans le processus d'origine : ce qu'il
    # écrit dans le dépôt et dans $QtRootDir appartient donc à l'utilisateur courant, et non à
    # l'administrateur comme ce serait le cas en relançant tout le script élevé.
    if($isAdmin -eq $true)
    {
        $process = Start-Process -FilePath $buildToolsBin -Wait -PassThru -ArgumentList $installArgs
    }
    else
    {
        Write-Host "   élévation nécessaire, accepter l'invite UAC qui va s'afficher..."

        try
        {
            $process = Start-Process -FilePath $buildToolsBin -Verb RunAs -Wait -PassThru -ArgumentList $installArgs
        }
        catch
        {
            throw "L'invite UAC a été refusée, et l'installation des Build Tools ne peut pas s'en passer. L'accepter, relancer depuis un PowerShell administrateur, ou passer -SkipBuildTools si MSVC est déjà là."
        }
    }

    # 3010 vaut succès, il signale seulement qu'un redémarrage est conseillé. Un code absent n'est pas
    # un échec : -Verb RunAs ne permet pas toujours de le relever sur un processus élevé. On laisse
    # alors juger l'Import-MsvcEnvironment qui suit, qui échoue de toute façon si rien n'est installé.
    if($null -ne $process.ExitCode -and $process.ExitCode -ne 0 -and $process.ExitCode -ne 3010)
    {
        throw "L'installation des Build Tools a échoué (code $($process.ExitCode))."
    }
}

# 2. Qt. La 5.15.2 est la dernière version dont les binaires sont librement téléchargeables, et
#    aqtinstall les récupère sans demander de compte Qt. qtmultimedia fait partie de l'installation
#    de base, seul qtwebengine doit être demandé en plus — et c'est lui qui impose MSVC, Chromium ne
#    se compilant pas avec MinGW.
Write-Host "== 2/5 Qt $QtVersion avec QtWebEngine"

if($SkipQt)
{
    Write-Host "   ignoré (-SkipQt)"
}
elseif(Test-Path (Join-Path $qtDir 'bin\qmake.exe'))
{
    Write-Host "   déjà installé dans $qtDir"
}
else
{
    $aqtBin = Join-Path $downloadDir 'aqt.exe'
    Get-FileIfNeeded -Url "https://github.com/miurahr/aqtinstall/releases/download/$AqtVersion/aqt_x64.exe" -Path $aqtBin

    Write-Host "   installation dans $QtRootDir (0,9 Go)..."

    # aqt écrit un aqtinstall.log dans le dossier courant : on se place dans build\bootstrap pour
    # qu'il y atterrisse avec le reste des téléchargements, et non à la racine du dépôt où il
    # apparaîtrait dans git status.
    Push-Location $downloadDir

    try
    {
        Invoke-BuildTool -Name 'aqt' -Command {
            & $aqtBin install-qt windows desktop $QtVersion $QtArch -m qtwebengine --outputdir $QtRootDir
        }
    }
    finally
    {
        Pop-Location
    }

    if(-not (Test-Path (Join-Path $qtDir 'bin\qmake.exe')))
    {
        throw "qmake est introuvable dans $qtDir après l'installation : vérifier -QtVersion et -QtArch."
    }
}

Import-MsvcEnvironment

# 3. Hunspell et zlib. Rien n'est fourni par le système sous Windows. Ce sont deux petites
#    bibliothèques sans dépendance : les compiler prend une quinzaine de secondes, contre une dizaine
#    de minutes et 912 Mo pour vcpkg, dont les deux tiers vont à libiconv qui ne sert à rien ici.
#    /MD est indispensable, c'est la bibliothèque C++ dynamique, celle qu'utilise Qt : avec /MT
#    l'édition de liens échouerait. On compile deux fois, release et debug : une bibliothèque release
#    seule suffisait à `nmake release`, mais faisait échouer `nmake debug` en LNK2038 sur
#    `RuntimeLibrary` et `_ITERATOR_DEBUG_LEVEL`, /MD et /MDd ne se mélangeant pas dans un même binaire.
Write-Host "== 3/5 Hunspell $HunspellVersion"

$hunspellLib = Join-Path $repoDir 'hunspell\lib\hunspell.lib'
$hunspellLibDebug = Join-Path $repoDir 'hunspell\lib\hunspelld.lib'

if((Test-Path $hunspellLib) -and (Test-Path $hunspellLibDebug))
{
    Write-Host "   déjà compilé"
}
else
{
    $archivePath = Join-Path $downloadDir "hunspell-$HunspellVersion.zip"
    Get-FileIfNeeded -Url "https://github.com/hunspell/hunspell/archive/refs/tags/v$HunspellVersion.zip" -Path $archivePath
    Expand-Archive $archivePath -DestinationPath $downloadDir -Force

    $sourceDir = Join-Path $downloadDir "hunspell-$HunspellVersion\src\hunspell"
    Write-Host "   compilation..."
    Push-Location $sourceDir

    try
    {
        # HUNSPELL_STATIC est nécessaire dès la compilation de Hunspell : sans lui son hunvisapi.h
        # déclare tout en __declspec(dllimport) et l'édition de liens échouera.
        Invoke-BuildTool -Name 'cl (hunspell)' -Command { & cl /nologo /c /O2 /MD /EHsc /DHUNSPELL_STATIC *.cxx }
        Invoke-BuildTool -Name 'lib (hunspell)' -Command { & lib /nologo /OUT:hunspell.lib *.obj }

        # Seconde passe pour hunspelld.lib. cl écrit toujours <source>.obj, il faut donc effacer les
        # objets release avant, sans quoi la seconde bibliothèque reprendrait les premiers. /Z7 plutôt
        # que /Zi : il range les symboles dans les .obj, qui les emportent dans le .lib, quand /Zi les
        # laisserait dans un vc140.pdb que personne ne copie et que l'éditeur de liens réclamerait
        # ensuite en LNK4099.
        Remove-Item *.obj -Force
        Invoke-BuildTool -Name 'cl (hunspell debug)' -Command { & cl /nologo /c /Od /MDd /Z7 /EHsc /DHUNSPELL_STATIC *.cxx }
        Invoke-BuildTool -Name 'lib (hunspell debug)' -Command { & lib /nologo /OUT:hunspelld.lib *.obj }
    }
    finally
    {
        Pop-Location
    }

    New-Item -ItemType Directory -Force -Path (Join-Path $repoDir 'hunspell\include\hunspell') | Out-Null
    New-Item -ItemType Directory -Force -Path (Join-Path $repoDir 'hunspell\lib') | Out-Null

    foreach($thisHeader in @('hunspell.hxx', 'hunspell.h', 'hunvisapi.h', 'atypes.hxx', 'w_char.hxx'))
    {
        Copy-Item (Join-Path $sourceDir $thisHeader) (Join-Path $repoDir 'hunspell\include\hunspell') -Force
    }

    Copy-Item (Join-Path $sourceDir 'hunspell.lib'), (Join-Path $sourceDir 'hunspelld.lib') (Join-Path $repoDir 'hunspell\lib') -Force
}

# zlib se compile deux fois pour la même raison que Hunspell, mais le symptôme est plus discret :
# étant en C, il n'emporte ni _ITERATOR_DEBUG_LEVEL ni l'enregistrement RuntimeLibrary que les
# en-têtes C++ posent, donc l'éditeur de liens ne peut pas rendre de LNK2038. Il ne reste que le
# /DEFAULTLIB:MSVCRT des objets release, qui dégénère en simple avertissement LNK4098 et fait cohabiter
# deux CRT dans le même binaire. C'est précisément le genre de mélange — allouer dans l'une, libérer
# dans l'autre — que la section « Corruption de tas sous Windows » de CLAUDE.md apprend à traquer : le
# laisser dans le binaire de débogage reviendrait à y introduire le défaut qu'on l'utilise à chercher.
Write-Host "== 4/5 zlib $ZlibVersion"

$zlibLib = Join-Path $repoDir 'zlib\lib\zlib.lib'
$zlibLibDebug = Join-Path $repoDir 'zlib\lib\zlibd.lib'

if((Test-Path $zlibLib) -and (Test-Path $zlibLibDebug))
{
    Write-Host "   déjà compilé"
}
else
{
    # L'archive de la release s'appelle zlib131.zip pour la 1.3.1 : le numéro y perd ses points.
    $archiveName = "zlib$($ZlibVersion -replace '\.', '').zip"
    $archivePath = Join-Path $downloadDir $archiveName
    Get-FileIfNeeded -Url "https://github.com/madler/zlib/releases/download/v$ZlibVersion/$archiveName" -Path $archivePath
    Expand-Archive $archivePath -DestinationPath $downloadDir -Force

    $sourceDir = Join-Path $downloadDir "zlib-$ZlibVersion"
    Write-Host "   compilation..."
    Push-Location $sourceDir

    try
    {
        Invoke-BuildTool -Name 'cl (zlib)' -Command { & cl /nologo /c /O2 /MD *.c }
        # zlib.lib est le nom que zlib.pri prend déjà par défaut sous MSVC, et celui que produit aussi
        # vcpkg : le choisir ici évite un ZLIB_LIB_NAME sur chaque appel à qmake. Il n'y a rien qui
        # oblige à ce nom, la recette est libre — c'est justement pourquoi autant prendre celui-là.
        Invoke-BuildTool -Name 'lib (zlib)' -Command { & lib /nologo /OUT:zlib.lib *.obj }

        # Seconde passe, comme pour Hunspell et avec le même Remove-Item pour la même raison.
        Remove-Item *.obj -Force
        Invoke-BuildTool -Name 'cl (zlib debug)' -Command { & cl /nologo /c /Od /MDd /Z7 *.c }
        Invoke-BuildTool -Name 'lib (zlib debug)' -Command { & lib /nologo /OUT:zlibd.lib *.obj }
    }
    finally
    {
        Pop-Location
    }

    New-Item -ItemType Directory -Force -Path (Join-Path $repoDir 'zlib\include') | Out-Null
    New-Item -ItemType Directory -Force -Path (Join-Path $repoDir 'zlib\lib') | Out-Null
    Copy-Item (Join-Path $sourceDir 'zlib.h'), (Join-Path $sourceDir 'zconf.h') (Join-Path $repoDir 'zlib\include') -Force
    Copy-Item (Join-Path $sourceDir 'zlib.lib'), (Join-Path $sourceDir 'zlibd.lib') (Join-Path $repoDir 'zlib\lib') -Force
}

# 5. OpenSSL. Sans lui le programme démarre mais ne peut joindre aucune page : Qt 5.15.2 charge
#    libssl-1_1-x64.dll et libcrypto-1_1-x64.dll à l'exécution, et en leur absence
#    QSslSocket::supportsSsl() est faux et tout échoue silencieusement. windeployqt ne les copie pas,
#    et Qt ne distribue plus que du OpenSSL 3, dont l'interface binaire est incompatible.
#    La 1.1.1 n'est plus maintenue depuis septembre 2023 : c'est un choix assumé faute d'alternative
#    simple, Qt 5.15.2 ne sachant pas parler à OpenSSL 3.
Write-Host "== 5/5 OpenSSL $OpenSslVersion"

$opensslDll = Join-Path $repoDir 'openssl\bin\libssl-1_1-x64.dll'

if(Test-Path $opensslDll)
{
    Write-Host "   déjà installé"
}
else
{
    $archivePath = Join-Path $downloadDir "openssl-$OpenSslVersion.zip"
    Get-FileIfNeeded -Url "https://download.firedaemon.com/FireDaemon-OpenSSL/openssl-$OpenSslVersion.zip" -Path $archivePath

    $hash = (Get-FileHash $archivePath -Algorithm SHA256).Hash

    if($hash -ne $OpenSslSha256)
    {
        throw "L'empreinte SHA-256 d'OpenSSL ne correspond pas : $hash au lieu de $OpenSslSha256. Fichier corrompu ou modifié, ne pas l'utiliser."
    }

    Write-Host "   empreinte SHA-256 vérifiée"
    $extractDir = Join-Path $downloadDir 'openssl'
    Expand-Archive $archivePath -DestinationPath $extractDir -Force

    New-Item -ItemType Directory -Force -Path (Join-Path $repoDir 'openssl\bin') | Out-Null
    Get-ChildItem $extractDir -Recurse -Filter '*-x64.dll' |
        Where-Object { $_.FullName -match '\\x64\\bin\\' } |
        Copy-Item -Destination (Join-Path $repoDir 'openssl\bin') -Force

    if(-not (Test-Path $opensslDll))
    {
        throw "libssl-1_1-x64.dll est introuvable après extraction : la disposition de l'archive FireDaemon a dû changer."
    }
}

if($KeepDownloads -eq $false)
{
    Remove-Item $downloadDir -Recurse -Force -ErrorAction SilentlyContinue
}

Write-Host ""
Write-Host "== Terminé. Vérification de ce qui est en place :"

$checks = [ordered]@{
    'qmake'          = Join-Path $qtDir 'bin\qmake.exe'
    'windeployqt'    = Join-Path $qtDir 'bin\windeployqt.exe'
    'hunspell.lib'   = $hunspellLib
    'hunspelld.lib'  = $hunspellLibDebug
    'zlib.lib'       = $zlibLib
    'zlibd.lib'      = $zlibLibDebug
    'libssl-1_1'     = $opensslDll
    'libcrypto-1_1'  = Join-Path $repoDir 'openssl\bin\libcrypto-1_1-x64.dll'
}

$missing = 0

foreach($thisCheck in $checks.GetEnumerator())
{
    if(Test-Path $thisCheck.Value)
    {
        Write-Host ("   ok      {0}" -f $thisCheck.Key)
    }
    else
    {
        Write-Host ("   MANQUE  {0} ({1})" -f $thisCheck.Key, $thisCheck.Value)
        $missing++
    }
}

if($missing -gt 0)
{
    throw "$missing élément(s) manquant(s) : relancer le script, il reprendra où il en était."
}

Write-Host ""
Write-Host "Pour fabriquer l'archive distribuable :"
Write-Host "    .\dist-windows.ps1 -QtDir $qtDir"
Write-Host ""
Write-Host "Pour compiler et essayer sans passer par l'archive, voir la section Windows du README."
