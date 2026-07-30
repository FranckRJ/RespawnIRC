# Compile RespawnIRC pour Windows, et ses tests avec -Tests. C'est le maillon qui manquait entre
# bootstrap-windows.ps1, qui installe les outils et s'arrête là, et dist-windows.ps1, qui fabrique
# l'archive : une machine vierge va maintenant du dépôt à l'exécutable sans qu'on tape une ligne de
# qmake. Le script de distribution l'appelle plutôt que de garder sa propre copie des mêmes étapes, et
# run-windows.ps1 l'appelle quand l'exécutable manque.
#
# Usage : .\build-windows.ps1 [-QtDir chemin\vers\Qt\5.15.2\msvc2019_64] [-Tests] [-Clean]
#         [-HunspellLibName hunspell-1.7] [-ZlibLibName zlibstatic]
# À défaut, le Qt utilisé est celui dont le qmake est dans le PATH.
#
# Les deux noms de bibliothèques ne servent qu'à ceux qui n'ont pas suivi la recette du README : les
# .pro prennent par défaut hunspell et, sous MSVC, zlib, qui sont les noms qu'elle produit. Le cas
# type est le hunspell-1.7 de vcpkg.
#
# Le script trouve tout seul l'environnement MSVC avec vswhere, il n'y a donc pas besoin de le lancer
# depuis une invite de commandes développeur.
#
# La compilation se fait hors des sources, dans build\respawnIrc et build\tests ; seuls les objets
# intermédiaires y restent, le DESTDIR des .pro déposant RespawnIRC.exe à la racine du dépôt et
# respawnIrcTests.exe dans build\. -Clean efface ces dossiers de compilation au lieu de les reprendre.
#
# La compilation de débogage n'est délibérément pas ici : elle demande ses propres noms de
# bibliothèques et son propre dossier de compilation (voir le README), et c'est un outil de
# diagnostic, pas un maillon de la chaîne qui va des sources à l'archive.

[CmdletBinding()]
param(
    [string]$QtDir,
    [string]$HunspellLibName,
    [string]$ZlibLibName,
    [switch]$Clean,
    [switch]$Tests
)

$ErrorActionPreference = 'Stop'

$repoDir = $PSScriptRoot

# Résolution de Qt, environnement MSVC et appel des outils, partagés avec les autres scripts Windows.
. (Join-Path $PSScriptRoot 'windows-common.ps1')

$qtDir = Resolve-QtDir -QtDir $QtDir
$qmakeBin = Join-Path $qtDir 'bin\qmake.exe'

if(-not (Test-Path $qmakeBin))
{
    throw "$qmakeBin est introuvable."
}

Import-MsvcEnvironment

# Une cible, c'est un dossier de compilation, un .pro, ses options qmake et l'exécutable attendu : le
# programme et les tests ne diffèrent que par ces quatre choses.
function Invoke-QmakeAndNmake
{
    param(
        [Parameter(Mandatory)][string]$BuildDir,
        [Parameter(Mandatory)][string]$ProFile,
        [Parameter(Mandatory)][string]$BuiltExe,
        [string[]]$Options = @()
    )

    if($Clean)
    {
        Remove-Item -Recurse -Force $BuildDir -ErrorAction SilentlyContinue
    }

    New-Item -ItemType Directory -Force -Path $BuildDir | Out-Null

    # L'exécutable en place est toujours effacé, et ce n'est pas un détail : le DESTDIR des .pro ne
    # distingue ni release ni debug ni dossier de compilation, si bien que le RespawnIRC.exe de la
    # racine peut venir d'ailleurs — d'un nmake debug, typiquement. nmake le comparerait alors à ses
    # objets, le trouverait plus récent, n'éditerait aucun lien et on continuerait d'exécuter ce
    # binaire-là en croyant l'avoir remplacé. Le retirer garantit que ce qui sort d'ici sort bien des
    # objets de ce dossier-ci, pour le prix d'une édition de liens.
    Remove-Item $BuiltExe -Force -ErrorAction SilentlyContinue

    Push-Location $BuildDir

    try
    {
        Invoke-BuildTool -Name 'qmake' -Command { & $qmakeBin $ProFile @Options }
        Invoke-BuildTool -Name 'nmake' -Command { nmake release }
    }
    finally
    {
        Pop-Location
    }

    # nmake rend 0 sans rien faire quand il juge sa cible à jour : c'est justement ce que l'effacement
    # ci-dessus évite, et le vérifier ferme la question au lieu de laisser la suite manipuler un
    # fichier absent.
    if(-not (Test-Path $BuiltExe))
    {
        throw "$BuiltExe n'a pas été produit alors que nmake a réussi."
    }
}

# Les noms de bibliothèques ne sont transmis que s'ils sont donnés : les valeurs par défaut des .pro
# sont déjà celles de la recette du README, et les répéter ne faisait rien. HUNSPELL_STATIC, lui, est
# nécessaire au Hunspell compilé à la main, dont les en-têtes déclareraient sinon tout en
# __declspec(dllimport) ; il est sans effet sur celui de vcpkg, qui engendre un hunvisapi.h au test
# déjà figé. Le passer toujours marche donc dans les deux cas.
$optionsForProgram = @('DEFINES+=HUNSPELL_STATIC')

if($HunspellLibName)
{
    $optionsForProgram += "HUNSPELL_LIB_NAME=$HunspellLibName"
}

if($ZlibLibName)
{
    $optionsForProgram += "ZLIB_LIB_NAME=$ZlibLibName"
}

Write-Host "== Compilation de RespawnIRC avec $qtDir"
Invoke-QmakeAndNmake -BuildDir (Join-Path $repoDir 'build\respawnIrc') `
    -ProFile (Join-Path $repoDir 'respawnIrc\respawnIrc.pro') `
    -BuiltExe (Join-Path $repoDir 'RespawnIRC.exe') `
    -Options $optionsForProgram

if($Tests)
{
    # tests.pro n'inclut que zlib.pri : ni Hunspell ni sa macro n'entrent dans les tests, et leur
    # passer des options qui ne les concernent pas ne ferait qu'entretenir la confusion.
    $optionsForTests = @()

    if($ZlibLibName)
    {
        $optionsForTests += "ZLIB_LIB_NAME=$ZlibLibName"
    }

    $testsExe = Join-Path $repoDir 'build\respawnIrcTests.exe'

    Write-Host "== Compilation des tests"
    Invoke-QmakeAndNmake -BuildDir (Join-Path $repoDir 'build\tests') `
        -ProFile (Join-Path $repoDir 'tests\tests.pro') `
        -BuiltExe $testsExe `
        -Options $optionsForTests

    Write-Host "== Vérifications"
    # Les tests sont liés à Qt comme le programme et ne démarrent pas sans ses DLL. Elles ne sont dans
    # le PATH que si l'appelant les y a mises, ce qu'on ne peut pas supposer puisque ce script sait
    # justement retrouver Qt tout seul.
    $env:PATH = "$qtDir\bin;$env:PATH"

    & $testsExe

    if($LASTEXITCODE -ne 0)
    {
        throw "Les vérifications ont échoué (code $LASTEXITCODE)."
    }
}
