such server is still not strong now, but at least it can do
1. create a thread once there's it "hear" another new client from its
   listen fd
3. when the stdout shows "there's a/an ..." means your instruction is
   sent and the server does take care it
4. 
	so, you can type the followings sequentially...
	//to thread1
	<signup>
	<username>Fridays<\>
	<password>yamatata<\>
	<login>
	<username>Friedchickendays<\>
	<password>heyhey<\>
	
	//to thread2
	<signup>
	<username>Friedchickendays<\>
	<password>heyhey<\>
	<login>
	<username>Fridays<\>
	<password>yamatata<\>
	
	//to thread1
	<knock>Fridays<\>
	
