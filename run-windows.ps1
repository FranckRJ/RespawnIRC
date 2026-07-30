# Lance le RespawnIRC fraîchement compilé, sans passer par l'archive de dist-windows.ps1.
#
# Usage : .\run-windows.ps1 [-QtDir chemin\vers\Qt\5.15.2\msvc2019_64] [-Logs]
#
# -Logs active RESPAWNIRC_DEBUG. Le paramètre ne peut pas s'appeler -Debug : c'est déjà le nom d'un
# paramètre commun ajouté par CmdletBinding, et PowerShell refuse le script au chargement.
#
# Le DESTDIR de respawnIrc.pro produit l'exécutable directement à la racine du dépôt, là où sont
# déjà resources\ et themes\ que pathTool::dataDirPath() va chercher à côté de lui : il n'y a donc
# rien à recopier, il suffit de mettre Qt et OpenSSL dans le PATH le temps de l'exécution.

[CmdletBinding()]
param(
    [string]$QtDir,
    [switch]$Logs
)

$ErrorActionPreference = 'Stop'

$repoDir = $PSScriptRoot
$builtExe = Join-Path $repoDir 'RespawnIRC.exe'

if(-not (Test-Path $builtExe))
{
    throw "$builtExe est introuvable : compilez d'abord (voir le README)."
}

# Résolution de Qt et vérification d'OpenSSL, partagées avec dist-windows.ps1.
. (Join-Path $PSScriptRoot 'windows-common.ps1')

$qtDir = Resolve-QtDir -QtDir $QtDir
# Sans -Required : l'absence d'OpenSSL n'empêche pas de lancer le programme, seulement de joindre une
# page, et c'est encore utile pour tout ce qui ne dépend pas du réseau.
$opensslDir = Get-OpenSslDir -RepoDir $repoDir

$env:PATH = "$qtDir\bin;$opensslDir;$env:PATH"

if($Logs)
{
    # Toutes les catégories respawnirc.* en debug, logs dans userdata\logs\respawnirc.log, et pages
    # non analysées sauvegardées dans userdata\logs\page-*.html.
    $env:RESPAWNIRC_DEBUG = '1'
    Write-Host "RESPAWNIRC_DEBUG actif, logs dans $(Join-Path $repoDir 'userdata\logs\respawnirc.log')"
}

& $builtExe
