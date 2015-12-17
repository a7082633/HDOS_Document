public String getMAC(String interspersed, Map data, int index) atr\citycode\wsjs\jyxh
			throws HSMException {
		String MAC1 =null;
		Socket Client;
		DataInputStream InputS;
		DataInputStream KeyS;
		PrintStream OutputS;
		String atr = data.get("atr").toString();
		String aab034 = str2HexStr(data.get("aab034").toString());
		//命令类型+命令+用户保留字+MAC类型+次主密钥索引+分散次数+密钥计算方法
		String s1 = "B080000000000000000000017F02";
		//分散持卡人密钥ATR + 城市代码 + 7378
		String s2 = aab034 + "7378"+atr+"00";
		//sessionKey数据=伪随机+交易序号
		String wsjs = data.get("wsjs").toString();
		String jyxh = data.get("jyxh").toString();
	    String s3 = wsjs + jyxh;
		//MAC初始数据+MAC 长度
	    String s4 = "0000000000000000"+"001A";
	    //MAC 数据 = 个人账户交易金额+个人自付金额+统筹基金支付金额+交易类型(32)+终端机编号+交易时间
	    String  yak065 = data.get("yka065").toString(); //4个字节金额
	    String  yka131 = data.get("yka131").toString(); //4字节金额
	    String  yka130 = data.get("yka130").toString(); //4字节金额
	    String  aae036 = data.get("aae036").toString(); //8个字节的时间
	    String  zdjbm =  data.get("zdjbm").toString(); //6 个字节
	    String s5 =(yak065+yka131+yka130+"32"+zdjbm+aae036);
	    
	    String inputString = s1 + s2 + s3 +s4 + s5;
	    
	    String bw = "B080000000000000000000017F02" +
	    		"3531303630307378" +
	    		"88653510600000001" +
	    		"00" +
	    		"00000000" +
	    		"11110006" +
	    		"0000000000000000" +
	    		"001A0000000000000000000000003251200081112720140820115100";
	    System.out.println(inputString);
	    System.out.println(inputString.length());
		byte[] recvbuf = new byte[17];
		int i = 0;
		try {
		
			Client = new Socket(address, port);
			InputS = new DataInputStream(Client.getInputStream());
			OutputS = new PrintStream(Client.getOutputStream());
			KeyS = new DataInputStream(System.in);
			OutputS.write(hexStringToBytes(inputString));
			i = InputS.read(recvbuf);
			System.out.println(bytesToHexString(recvbuf));
		    MAC1 = bytesToHexString(recvbuf).substring(18,34).toUpperCase();
		    
		} catch (IOException e) {
			throw new HSMException("IOException Happened" + e.getMessage());
		}
		return MAC1;
	}





B080000000000000000000 0173 02 34323031303073788684420100000135 00 D671B973D4033B01 0000000000000000 10 842400010C ba7cb5d03e93d00e a15d623b7b3edf48800000"
