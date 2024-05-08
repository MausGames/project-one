@ECHO OFF

CALL ..\..\CoreEngine\tools\scripts\download_archive.bat project-one launcher "..\executable"
CALL ..\..\CoreEngine\tools\scripts\download_archive.bat project-one fonts "..\executable\data\fonts"
CALL ..\..\CoreEngine\tools\scripts\download_archive.bat project-one models "..\executable\data\models"
CALL ..\..\CoreEngine\tools\scripts\download_archive.bat project-one sounds "..\executable\data\sounds"
CALL ..\..\CoreEngine\tools\scripts\download_archive.bat project-one textures "..\executable\data\textures"