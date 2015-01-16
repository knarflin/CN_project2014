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
	
	//to thread1
	<username>Fridays<\>
	<message>Yes, i can transmit message from Fridays to Friedchickendays~~<\>
	
	//------------------------------------//
	//to thread1
	<signup>
	<username>Fridays<\>
	<password>yamatata<\>
	<signup>
	<username>Friedchickendays<\>
	<password>heyhey<\>
	<signup>
	<username>Mondays<\>
	<password>mondays<\>
	<login>
	<username>Friedchickendays<\>
	<password>heyhey<\>
	
	<username>Fridays<\>
	<message>Yes, i can transmit message from Fridays to Friedchickendays~~<\>
	
	<username>Fridays<\>
	<message>can you lend me Ur girlfriend, pls~~<\>
	
	<username>Fridays<\>
	<message>can you lend me Ur OOXX, pls~~<\>
	
	<username>Fridays<\>
	<filedata,CN.exe,15>Yes, file from Friedchickendays to Fridays<\>
	
	<username>Fridays<\>
	<filedata,CN.exe,1>Yes, first file<\>
	
	//to thread2
	<login>
	<username>Mondays<\>
	<password>mondays<\>
	
	<username>Fridays<\>
	<message>hey Im Mondays~~<\>
	
	<username>Fridays<\>
	<message>hey Im Nothing~~<\>
	
	<username>Fridays<\>
	<message>can you lend me some bananas, pls~~<\>
	
	//to thread3
	<login>
	<username>Fridays<\>
	<password>yamatata<\>
	
	//to thread1
	<knock>Fridays<\>
