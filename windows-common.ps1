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

# Le dossier de Qt donné en argument, ou celui déduit du qmake du PATH : c'est celui qui contient
# bin\qmake.exe, donc deux niveaux au-dessus de l'exécutable.
function Resolve-QtDir
{
    param([string]$QtDir)

    if($QtDir)
    {
        return $QtDir
    }

    if(Get-Command qmake -ErrorAction SilentlyContinue)
    {
        return Split-Path (Split-Path (Get-Command qmake).Source)
    }

    throw "Qt introuvable : passez son chemin avec -QtDir, ou mettez son qmake dans le PATH."
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
