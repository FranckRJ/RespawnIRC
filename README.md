# RespawnIRC

Logiciel offrant une interface de client IRC pour les forums de jeuxvideo.com.

Lien de téléchargement : https://github.com/FranckRJ/RespawnIRC/releases/latest. La version Windows demande Windows 10 1809 ou plus récent, en 64 bits.

Message de présentation sur jeuxvideo.com : http://www.jeuxvideo.com/forums/42-1000021-40573812-1-0-1-0-pc-android-respawnirc.htm#post_731107420.

Pour plus d'infos, le site : https://pijon.fr/RespawnIRC/.

## Compilation

Cette section doit être ré-écrite, mais pour faire simple, exécutez ces commandes:

```shell
# Va télécharger toutes les dépendances et le compilateur (fait uniquement pour
# les composants non présents dans le PATH). Pour installer manuellement les
# dépendances voir plus bas.
powershell -ExecutionPolicy Bypass -File .\bootstrap-windows.ps1

# Pour compiler en lançant les tests.
powershell -ExecutionPolicy Bypass -File .\build-windows.ps1 -Tests

# Pour exécuter le programme compiler.
powershell -ExecutionPolicy Bypass -File .\run-windows.ps1

# Pour distribuer le programme (générer un .zip distribuable).
powershell -ExecutionPolicy Bypass -File .\dist-windows.ps1
```

Si vous voulez installer vous-même les dépendances, téléchargez MSVC / Qt 6 / Hunspell et zlib.  
Pour MSVC et Qt, ils doivent simplement être dans le PATH. Qt (msvc2022_64) a besoin du module QtWebEngine.  
Pour les bibliothèques, les fichiers d'Hunspell et zlib doivent être dans hunspell/lib / hunspell/include et zlib/lib / zlib/include respectivement. Ces chemins sont relatifs à la racine du projet.

La section Linux / macOS est encore à faire, vous pouvez essayez de vous débrouiller avec des scripts unix / macos à la racine du projet ou (si vous avez la foi), lire la monstruosité pondue par claude (au moins c'est complet): https://github.com/FranckRJ/RespawnIRC/blob/85015351e90c271a8a4eadb4f03d722d7c0d3f02/README.md
