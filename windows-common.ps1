# Ce que build-windows.ps1, dist-windows.ps1 et run-windows.ps1 ont en commun : retrouver
# l'installation de Qt, savoir si OpenSSL est là, charger l'environnement de MSVC et appeler un outil
# de compilation. Les deux premiers blocs étaient recopiés mot pour mot dans les scripts de
# distribution et de lancement, à la seule différence du throw contre le Write-Warning, d'où le
# -Required ci-dessous ; les deux autres attendaient ici le troisième utilisateur que le script de
# compilation vient d'apporter.
#
# Se charge par point-sourcing, pour que les fonctions atterrissent dans la portée de l'appelant :
#     . (Join-Path $PSScriptRoot 'windows-common.ps1')
#
# bootstrap-windows.ps1 ne charge rien d'ici et garde ses propres copies d'Import-MsvcEnvironment et
# d'Invoke-BuildTool : il tourne sur une machine où rien n'est encore installé, et son autonomie a une
# valeur propre. C'est le seul des quatre scripts à avoir une raison de ne dépendre de rien, et sa
# duplication est délibérée.
#
# Ce fichier est en UTF-8 avec BOM comme les autres .ps1 du dépôt : PowerShell 5.1 lit un .ps1 comme
# de l'ANSI sans lui, et tous les accents des messages sont abîmés.

# Là où bootstrap-windows.ps1 installe Qt, et donc là où le chercher quand personne ne l'a désigné.
# C'est son -QtRootDir, dont c'est le défaut ; un Qt installé ailleurs se désigne par -QtDir, comme
# sous Unix.
$qtRootDirForSearch = 'C:\Qt'

# Ce qui cloche avec le Qt dont on donne le qmake, s'il cloche quelque chose : 0 s'il convient, 1 si
# son qmake ne s'exécute pas, 2 s'il n'a pas QtWebEngine. C'est le pendant exact de la qtSuitability
# d'unix-common.sh, et le module se cherche de la même façon — en demandant son dossier à qmake plutôt
# qu'en devinant un chemin.
#
# Le cas n'a rien de théorique ici : le Qt pour MinGW n'a pas QtWebEngine, Chromium ne se compilant
# qu'avec MSVC, et c'est le premier que trouvent ceux qui ont installé Qt par son installateur en
# ligne sans y prendre garde. Sans ce test, son qmake était accepté et l'échec n'arrivait qu'au
# « Unknown module(s) in QT: webenginewidgets », qui ne dit ni quel Qt a été pris, ni pourquoi
# celui-là n'ira jamais.
function Get-QtSuitability
{
    param([Parameter(Mandatory)][string]$QmakeBin)

    $archDataDir = $null
    $previousPreference = $ErrorActionPreference
    $ErrorActionPreference = 'Continue'

    try
    {
        $archDataDir = & $QmakeBin -query QT_INSTALL_ARCHDATA 2>$null | Select-Object -First 1
    }
    catch
    {
        $archDataDir = $null
    }
    finally
    {
        $ErrorActionPreference = $previousPreference
    }

    if(-not $archDataDir)
    {
        return 1
    }

    if(-not (Test-Path (Join-Path $archDataDir 'mkspecs\modules\qt_lib_webenginewidgets.pri')))
    {
        return 2
    }

    return 0
}

# Les Qt où regarder quand l'appelant n'en a désigné aucun, dans l'ordre : celui du PATH d'abord, puis
# ceux de C:\Qt. Ce second endroit est ce qui manquait — bootstrap-windows.ps1 installe Qt là et ne
# touche pas au PATH, si bien qu'un `.\dist-windows.ps1` sans argument échouait sur « Qt introuvable »
# au sortir d'un amorçage qui venait pourtant de l'installer. C'est le pendant du ~/Qt/*/clang_64
# qu'unix-common.sh regarde sous macOS, et pour la même raison : le défaut des scripts doit tomber sur
# le Qt que le README fait installer.
#
# L'ordre des versions est celui du tri, donc 5.15.2 avant un 6.x, ce qui est le bon défaut pour ce
# dépôt. msvc*_64 exclut au passage les Qt 32 bits et ceux pour MinGW, mais ce n'est pas là-dessus
# qu'on compte : c'est Get-QtSuitability qui écarte un Qt sans QtWebEngine, y compris celui du PATH.
function Get-CandidateQtDirs
{
    $candidates = @()

    if(Get-Command qmake -ErrorAction SilentlyContinue)
    {
        $candidates += Split-Path (Split-Path (Get-Command qmake).Source)
    }

    foreach($thisDir in @(Get-Item (Join-Path $qtRootDirForSearch '*\msvc*_64') -ErrorAction SilentlyContinue | Sort-Object FullName))
    {
        if(Test-Path (Join-Path $thisDir.FullName 'bin\qmake.exe'))
        {
            $candidates += $thisDir.FullName
        }
    }

    return $candidates
}

# Dit ce qui manque et comment l'obtenir. Le message vaut la peine d'être écrit : sans lui, l'absence
# du module ne se voit qu'à l'erreur de qmake, qui ne dit pas lequel prendre.
function Get-QtWithoutWebEngineMessage
{
    param([string]$QtDir)

    if($QtDir)
    {
        $message = "Ce Qt n'a pas QtWebEngine, dont RespawnIRC a besoin : $QtDir."
    }
    else
    {
        $message = "Aucun Qt avec QtWebEngine trouvé, et RespawnIRC en a besoin."
    }

    return "$message QtWebEngine n'existe pas pour MinGW, Chromium ne se compilant qu'avec MSVC : il faut le Qt 5.15.2 msvc2019_64, que bootstrap-windows.ps1 installe dans $qtRootDirForSearch (voir le README). Une fois là, les scripts le trouvent seuls ; sinon, désignez-le avec -QtDir."
}

# Le dossier de Qt donné en argument, ou le premier candidat qui convienne : c'est celui qui contient
# bin\qmake.exe, donc deux niveaux au-dessus de l'exécutable.
#
# Un Qt désigné à la main n'est jamais remplacé en douce par un autre : s'il ne convient pas, on le dit
# plutôt que d'aller compiler contre un Qt que personne n'a demandé. C'est le principe qu'unix-common.sh
# applique déjà, et il compte double ici, où le Qt résolu sert aussi à windeployqt.
function Resolve-QtDir
{
    param([string]$QtDir)

    if($QtDir)
    {
        $qmakeBin = Join-Path $QtDir 'bin\qmake.exe'

        if(-not (Test-Path $qmakeBin))
        {
            throw "$qmakeBin est introuvable."
        }

        $suitability = Get-QtSuitability -QmakeBin $qmakeBin

        if($suitability -eq 1)
        {
            throw "Ce qmake ne s'exécute pas : $qmakeBin."
        }

        if($suitability -eq 2)
        {
            throw (Get-QtWithoutWebEngineMessage -QtDir $QtDir)
        }

        return $QtDir
    }

    $candidates = @(Get-CandidateQtDirs)

    # Le premier qmake qui n'a pas démarré, gardé pour le cas où aucun candidat ne convient : c'est une
    # cause plus précise que « aucun Qt avec QtWebEngine », et elle enverrait chercher ailleurs.
    $qmakeThatDoesNotRun = ''

    foreach($thisCandidate in $candidates)
    {
        $suitability = Get-QtSuitability -QmakeBin (Join-Path $thisCandidate 'bin\qmake.exe')

        if($suitability -eq 0)
        {
            return $thisCandidate
        }

        if($suitability -eq 1 -and -not $qmakeThatDoesNotRun)
        {
            $qmakeThatDoesNotRun = Join-Path $thisCandidate 'bin\qmake.exe'
        }
    }

    if($candidates.Count -eq 0)
    {
        throw "Qt introuvable : passez son chemin avec -QtDir, mettez son qmake dans le PATH, ou installez-le avec bootstrap-windows.ps1, qui le pose dans $qtRootDirForSearch."
    }

    if($qmakeThatDoesNotRun)
    {
        throw "Ce qmake ne s'exécute pas : $qmakeThatDoesNotRun."
    }

    throw (Get-QtWithoutWebEngineMessage -QtDir '')
}

# Le dossier où le dépôt attend OpenSSL. Qt 5.15.2 charge libssl-1_1-x64.dll et libcrypto-1_1-x64.dll
# à l'exécution pour tout ce qui est HTTPS : en leur absence QSslSocket::supportsSsl() est faux et
# aucune page de jeuxvideo.com n'est joignable, sans message clair. windeployqt ne les copie pas et Qt
# ne les distribue plus (voir le README). Avec -Required, leur absence est une erreur — une archive
# sans elles ne servirait à rien ; sans, un simple avertissement, le programme démarrant quand même.
function Get-OpenSslDir
{
    param([Parameter(Mandatory)][string]$RepoDir, [switch]$Required)

    $opensslDir = Join-Path $RepoDir 'openssl\bin'

    if(-not (Test-Path (Join-Path $opensslDir 'libssl-1_1-x64.dll')))
    {
        if($Required)
        {
            throw "OpenSSL introuvable dans openssl\bin : sans lui le programme ne peut joindre aucune page (voir le README)."
        }

        Write-Warning "OpenSSL absent d'openssl\bin : le programme démarrera mais ne pourra joindre aucune page."
    }

    return $opensslDir
}

# Charge les variables d'environnement de MSVC (cl, nmake, rc, dumpbin) dans la session courante :
# vcvars64.bat les pose dans son propre processus, on les récupère en lisant son `set` final. C'est ce
# qui évite d'avoir à lancer les scripts depuis une invite de commandes développeur. La fonction ne
# fait rien si nmake répond déjà, ce qui la rend sans coût quand un script en appelle un autre.
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
