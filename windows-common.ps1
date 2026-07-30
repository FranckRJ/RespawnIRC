# Le peu que dist-windows.ps1 et run-windows.ps1 ont en commun : retrouver l'installation de Qt, et
# savoir si OpenSSL est là. Les deux blocs étaient recopiés mot pour mot dans les deux scripts, à la
# seule différence du throw contre le Write-Warning, d'où le -Required ci-dessous.
#
# Se charge par point-sourcing, pour que les fonctions atterrissent dans la portée de l'appelant :
#     . (Join-Path $PSScriptRoot 'windows-common.ps1')
#
# bootstrap-windows.ps1 ne charge rien d'ici et garde ses propres copies d'Import-MsvcEnvironment et
# d'Invoke-BuildTool : il tourne sur une machine où rien n'est encore installé, et son autonomie a une
# valeur propre. C'est le seul des trois scripts à avoir une raison de ne dépendre de rien, et sa
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
