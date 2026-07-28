# Lance le RespawnIRC fraîchement compilé, sans passer par l'archive de dist-windows.ps1.
#
# Usage : .\run-windows.ps1 [-QtDir chemin\vers\Qt\5.15.2\msvc2019_64] [-Logs]
#
# -Logs active RESPAWNIRC_DEBUG. Le paramètre ne peut pas s'appeler -Debug : c'est déjà le nom d'un
# paramètre commun ajouté par CmdletBinding, et PowerShell refuse le script au chargement.
#
# L'exécutable produit dans build\ ne peut pas être lancé sur place : il lui manque les DLL de Qt,
# celles d'OpenSSL, et surtout les dossiers resources\ et themes\ que pathTool::dataDirPath() va
# chercher à côté de lui. Ce script le recopie donc à la racine du dépôt, où ces dossiers sont déjà,
# et met Qt et OpenSSL dans le PATH le temps de l'exécution.
#
# Ne pas remplacer cette copie par une jonction vers resources\ dans build\ : dist-windows.ps1
# supprime build\respawnIrc récursivement, et PowerShell 5.1 suit les jonctions en supprimant.

[CmdletBinding()]
param(
    [string]$QtDir,
    [switch]$Logs
)

$ErrorActionPreference = 'Stop'

$repoDir = $PSScriptRoot
$builtExe = Join-Path $repoDir 'build\respawnIrc\release\RespawnIRC.exe'

if(-not (Test-Path $builtExe))
{
    throw "$builtExe est introuvable : compilez d'abord (voir le README)."
}

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
    throw "Qt introuvable : passez son chemin avec -QtDir, ou mettez son qmake dans le PATH."
}

$opensslDir = Join-Path $repoDir 'openssl\bin'

if(-not (Test-Path (Join-Path $opensslDir 'libssl-1_1-x64.dll')))
{
    Write-Warning "OpenSSL absent d'openssl\bin : le programme démarrera mais ne pourra joindre aucune page."
}

Copy-Item $builtExe $repoDir -Force

$env:PATH = "$qtDir\bin;$opensslDir;$env:PATH"

if($Logs)
{
    # Toutes les catégories respawnirc.* en debug, logs dans logs\respawnirc.log, et pages non
    # analysées sauvegardées dans logs\page-*.html.
    $env:RESPAWNIRC_DEBUG = '1'
    Write-Host "RESPAWNIRC_DEBUG actif, logs dans $(Join-Path $repoDir 'logs\respawnirc.log')"
}

& (Join-Path $repoDir 'RespawnIRC.exe')
