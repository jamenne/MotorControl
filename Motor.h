//
//  Motor.h
//
//
//  Created by Janine Müller on 30.07.2015.
//
//

#ifndef ____Motor__
#define ____Motor__

#include <vector>
#include <string>

//Opcodes of all TMCL commands that can be used in direct mode
#define TMCL_ROR 1
#define TMCL_ROL 2
#define TMCL_MST 3
#define TMCL_MVP 4
#define TMCL_SAP 5
#define TMCL_GAP 6
#define TMCL_STAP 7
#define TMCL_RSAP 8
#define TMCL_SGP 9
#define TMCL_GGP 10
#define TMCL_STGP 11
#define TMCL_RSGP 12
#define TMCL_RFS 13
#define TMCL_SIO 14
#define TMCL_GIO 15
#define TMCL_SCO 30
#define TMCL_GCO 31
#define TMCL_CCO 32

//Opcodes of TMCL control functions (to be used to run or abort a TMCL program in the module)
#define TMCL_APPL_STOP 128
#define TMCL_APPL_RUN 129
#define TMCL_APPL_RESET 131

//Options for MVP commands
#define MVP_ABS 0
#define MVP_REL 1
#define MVP_COORD 2

//Options for RFS command
#define RFS_START 0
#define RFS_STOP 1
#define RFS_STATUS 2

#define FALSE 0
#define TRUE 1

//Result codes for GetResult
#define TMCL_RESULT_OK 0
#define TMCL_RESULT_NOT_READY 1
#define TMCL_RESULT_CHECKSUM_ERROR 2

class Motor {

	public:
		Motor(unsigned char &Address, unsigned char &Status, int &Value, bool verbosity = false);
		virtual ~Motor(){};

	    void CheckComports();
	    void ConnectMotor(bool verbosity);
	    void ReferenceRunX();
	    void ReferenceRunY();
	    void MoveRelative(std::string xy, int pos, int speed=100);
	    void MoveAbsolute(std::string xy, int pos, int speed=100);
	    int CalcStepsX(double pos);
	    int CalcStepsY(double pos);

	private:
		int _NumOfMotor;
		std::vector<int> _Motor_storage;
		unsigned char &_Address;
		unsigned char &_Status;
		int &_Value;
		void SendCmd(int Handle, unsigned char Address, unsigned char Command, unsigned char Type, unsigned char Motor, int Value);
		unsigned char GetResult(int Handle, unsigned char Address, unsigned char Status, int Value); 

};  

#endif /* defined(____Motor__) */