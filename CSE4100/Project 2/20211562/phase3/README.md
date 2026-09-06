make 후 ./myShell 입력 시 실행되며 
fg, bg, kill, jobs 명령어가 추가되어 기존 UNIX shell과 동일하게 동작하도록 하였으며 
명령어 뒤에 &을 붙여 입력하면(마지막 argument에 붙어있든 공백으로 떨어져있든) 해당 명령어가 background process로 실행되게 했습니다.
foreground process와 background process가 각각 의도된 대로 정상적으로 동작합니다.