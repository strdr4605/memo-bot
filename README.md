# memo-bot

Expected to be a slack bot

```text
User: /new-memo @user1 "Laughed"
MemoBot: Event:
            eventId1: @user1 Laughed 1 time
User: /new-memo @user2 "Screamed"
MemoBot: Event:
            eventId2: @user2 Screamed 1 time
User: /memo eventId2
MemoBot: Event:
            eventId2: @user2 Screamed 2 times
User: /memos
MemoBot: Events:
            eventId1: @user1 Laughed 1 time
            eventId2: @user2 Screamed 2 times
```

## Build + Watch

```bash
npm start
```

## Editor

If you use `vscode`, Press `Windows + Shift + B` it will build automatically
