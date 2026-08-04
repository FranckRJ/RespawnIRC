# Numéro de version de RespawnIRC, et seul endroit où il est écrit. Il était auparavant dans le
# littéral C++ de respawnIrc.cpp, que trois analyseurs syntaxiques allaient lire : une expression
# rationnelle PowerShell dans dist-windows.ps1, un sed dans dist-macos.sh, et un enchaînement
# $$cat/$$find/$$replace dans respawnIrc.pro pour le bundle macOS.
#
# Il part maintenant d'ici vers ses trois destinations : respawnIrc.pro le pousse dans un DEFINES que
# respawnIrc.cpp reprend, s'en sert pour la VERSION que qmake écrit dans Info.plist, et les deux
# scripts de distribution lisent ce fichier-ci pour nommer l'archive.
#
# Conséquence à connaître : le programme ne se compile plus sans qmake, le numéro n'étant plus dans
# les sources. C'est sans objet ici, qmake étant le seul système de compilation du dépôt.

RESPAWNIRC_VERSION = 3.1.17
