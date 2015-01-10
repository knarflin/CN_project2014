such server is still not strong now, but at least it can do
0. just one way message from client to server, that is, it wont return say <signup-U-OK>, <signup-FAIL> to any given client
1. create a thread once there's it "hear" another new client from its
   listen fd
2. login/sinup is ok now; but U'll find out U CANT logout...
3. when the stdout shows "there's a/an ..." means your instruction is
   sent and the server does take care it
4. can just accept input like
        <signup>
        <username>Fridays<\>
   but not
        <signup><username>Fridays<\>
        which will be refined later

	so, you can type the followings sequentially...
	<signup>
	<username>Fridays<\>
	<password>yamatata<\>
	<signup>
	<username>Fridays<\>
	<password>yamatata<\>
	<login>
	<username>littleJohn<\>
	<password>???<\>
	<logout>
