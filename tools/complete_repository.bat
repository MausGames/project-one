@ECHO OFF

CALL ..\..\core-engine\tools\scripts\download_archive.bat project-one launcher "..\executable"
CALL ..\..\core-engine\tools\scripts\download_archive.bat project-one fonts "..\executable\data\fonts"
CALL ..\..\core-engine\tools\scripts\download_archive.bat project-one models "..\executable\data\models"
CALL ..\..\core-engine\tools\scripts\download_archive.bat project-one sounds "..\executable\data\sounds"
CALL ..\..\core-engine\tools\scripts\download_archive.bat project-one textures "..\executable\data\textures"