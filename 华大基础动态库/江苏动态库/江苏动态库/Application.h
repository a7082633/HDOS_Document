#ifndef APPLICATION_H
#define APPLICATION_H

//#include "SI_ICCReaderHD.h"
int wom=0;

typedef struct struKeyRelation{
	char cDDFName[5];		//对应用户卡内DDF区
	char cAppName[5];		//对应用户卡内目录应用区
	char cKeyName[10];		//对应用户卡内密钥名称，如RK1DF02
	unsigned char cCardId[3];	//对应用户卡内密钥标识
	unsigned char cPsamType[3];//对应PSAM卡内用途
	unsigned char cPsamId[3];	//对应PSAM卡内标识
	BYTE cPsamLevel;			//对应分散级别，1,2,3级
}KEYRELATION;

static KEYRELATION  g_KeyRelation[]= {

	{"3F00","DDF1","IRK",       "00","67","08",1},
	{"3F00","DDF1","BK",		"05","28","11",3},
	{"3F00","DDF1","PUK",		"06","28","10",3},
	{"3F00","DDF1","UKMF",   	"04","28","12",3},
	{"3F00","DDF1","RKMFEF0C",  "07","48","5D",2},//5

//	{"3F00","DDF1","STKDDF1",	"02","48","09",3},
	{"DDF1","DF01","UK1DF01",	"83","28","42",3},
	{"DDF1","DF01","UK2DF01",	"84","28","43",3},
	{"DDF1","DF01","UK3DF01",	"85","27","24",3},
	{"DDF1","DF01","UK4DF01",	"86","27","25",3},
	{"DDF1","DF01","UK5DF01",	"87","28","44",3},//5

	{"DDF1","DF02","UK1DF02",	"83","27","26",3},
	{"DDF1","DF02","UK2DF02",	"84","28","45",3},
	{"DDF1","DF02","UK3DF02",	"85","28","46",3},
	{"DDF1","DF02","UK4DF02",	"86","27","27",3},
	{"DDF1","DF02","UK5DF02",	"87","27","28",3},//5

	{"DDF1","DF03","LKDF03",	"83","28","40",3},
	{"DDF1","DF03","UK1DF03",	"86","28","47",3},
	{"DDF1","DF03","UK2DF03",	"87","28","29",3},
	{"DDF1","DF03","UK3DF03",	"88","28","48",3},
	{"DDF1","DF03","UK4DF03",	"8C","28","58",3},
	{"DDF1","DF03","UK5DF03",	"89","28","59",3},
	{"DDF1","DF03","UK6DF03",	"8A","28","5A",3},
	{"DDF1","DF03","UK7DF03",	"8B","28","5B",3},
	{"DDF1","DF03","RK1DF03",	"84","67","2B",1},
	{"DDF1","DF03","RK2DF03",	"85","67","2C",1},//10

	{"DDF1","DF04","LKDF04",	"83","28","41",3},
	{"DDF1","DF04","UK1DF04",	"85","28","49",3},
	{"DDF1","DF04","UK2DF04",	"86","27","2a",3},
	{"DDF1","DF04","UK4DF04",	"88","28","54",3},
	{"DDF1","DF04","UK6DF04",	"87","28","5C",3},
	{"DDF1","DF04","RK1DF04",	"84","67","2D",1}//6
};

typedef struct tagFILE_KEY_RELATION {
	char cDFId[5];		//对应用户卡内目录标识
	char cFileId[5];		//对应用户卡内文件标识
	char cRkeyName[12];	//对应该文件的读密钥，0表示free，PIN表示用PIN
	char cWkeyName[10];	//对应该文件的写密钥
} FILE_KEY_RELATION;

FILE_KEY_RELATION g_FileKeyRelation[]={

	{"SSSE","EF05","0","UKDDF1"},
	{"SSSE","EF06","0","UKDDF1"},
	{"SSSE","EF07","0","UKDDF1"},
	{"SSSE","EF0C","RKDDF1","UKDDF1"},
	{"DF01","EF05","PIN","UK1DF01"},
	{"DF01","EF06","PIN","UK4DF01"},

	{"DF01","EF07","PIN|UK2DF01","UK2DF01"},	//PIN或UK2DF01
	{"DF01","EF08","PIN","UK3DF01"},
	{"DF01","EF09","PIN|UK2DF01","UK2DF01"},	//PIN或UK2DF01
	{"DF01","EF0A","PIN","UK5DF01"},
	{"DF02","EF05","PIN","UK1DF02"},
	{"DF02","EF06","PIN|UK2DF02","UK2DF02"},	//PIN或UK2DF02
	{"DF02","EF07","PIN|UK3DF02","UK3DF02"},	//PIN或UK3DF02
	{"DF02","EF08","PIN|UK4DF02","UK4DF02"},	//PIN或UK4DF02
	{"DF02","EF09","PIN|UK5DF02","UK5DF02"},	//PIN或UK5DF02
	{"DF03","EF05","PIN&RK1DF03","UK1DF03"},	//PIN并且UK1DF03
	{"DF03","EF06","PIN","UK2DF03"},
	{"DF03","EF07","PIN&RK1DF03","UK3DF03"},	//PIN并且RK1DF03
	{"DF03","EF0C","PIN","UK4DF03"},	
	{"DF03","EF0D","PIN","UK5DF03"},	//
	{"DF03","EF0E","PIN","UK6DF03"},	//
	{"DF03","EF0F","PIN","UK7DF03"},	//
	{"DF04","EF05","PIN","UK1DF04"},
	{"DF04","EF06","PIN&RK1DF04","UK2DF04"},	//PIN并且UK2DF04
	{"DF04","EF08","PIN","COS"},		
	{"DF04","EF0C","PIN","UK6DF04"},
	{"DF04","EF0D","PIN","UK4DF04"},
/*
	{"DDF4","0000","EF05","","UKDDF4"},
	{"DDF4","0000","EF06","","UKDDF4"},
	{"DDF4","DF01","EF05","PIN","UK1DF01"},
	{"DDF4","DF02","EF05","RK1DF02","UK1DF02"},
	{"DDF4","DF02","EF06","RK1DF02","UK2DF02"},
	{"DDF4","DF03","EF05","PIN","UK1DF03"},
	{"DDF4","DF03","EF06","PIN","UK2DF03"},
	{"DDF4","DF04","EF05","PIN","UK1DF04"},
	{"DDF4","DF04","EF06","PIN","UK2DF04"},*/

	};

#endif