/*  TITLE
	Path Oriented DEcison Making (PODEM)
*/

/*  PERSONAL DETAILS
	Krishna Prasad Suresh
	MS in ECE at Georgia Institute of Technology
	Specialization: VLSI Systems and Logic Design
	skprasad.22@gmail.com
	+1 (470) 263-0867
*/

/*  DESCRIPTION
	PODEM is a Test Generation Algorithm characterized by a direct search process, in which decisions consist of only PI assignments.

	PODEM treats a value vk to be justified for line k as an OBJECTIVE (k, vk) to be achieved via PI assignments.
	A backtracking procedure maps a desired objective into a PI assignment that is likely to contribute to achieving the objective.
	The PI assignment returned by Backtrace (k, vk) is (j, vj). An x-path from k to j is chosen.
	No values are assigned during backtracing. Values are assigned only by simulating PI assignments, done by Impl().y
*/

/*  STEPS
	1. Put the text file containing the circuit in the same folder as that of the code.
	2. Enter the filename without .txt.
	3. Proceed further based on the options you select.
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct NodenValue
{
	int nodeNumber;
	int nodeValue;
};

struct Gates
{
	string	gateType;
	int		gateNumber;
	int		gateInput1, gateInput2;
	int		gateOutput;
	int		gateFlag;
};

struct Nodes
{
	int nodeValue;
	int nodeFlag;
	int faultFlag;
	vector <Gates>	listofGates;
};

// Global variables declaration
NodenValue Fault;
NodenValue PIObjective;
NodenValue PIBacktrace;
NodenValue PISet;
Gates G;
Nodes N;

int faultActFlag = 0;
int status = 0;

vector <Gates> gate;
vector <Nodes> node;
vector <int> PINodes;
vector <int> PONodes;

class PODEM_functions
{
public:
	PODEM_functions();
	~PODEM_functions();

	// Reads the file
	int readFile()
	{
		int faultNode = 0;
		int faultValue = 0;
		int i = 0;
		int no_spaces1 = 0;
		int lineCount = 0;

		ostringstream s1;
		ostringstream s2;

		string outputFileName = "";
		string inputFileName = "";
		string line = "";

		cout << "Enter the filename here: ";			// Name of the input file
		cin  >> inputFileName;

		inputFileName += ".txt";

		ifstream inputFile(inputFileName);

		// Displays the contents of the input file
		if(inputFile.is_open())
		{
			while(getline(inputFile, line))
				cout << endl << line;
			inputFile.close();
		}

		else
		{
			cout << endl << "Unable to open file" << endl;
			return 0;
		}

		inputFile.open(inputFileName);

		if (inputFile.is_open())
		{
			while (getline(inputFile, line))
			{	
				int no_spaces = std::count(line.begin(), line.end(), ' ');				// Number of spaces in each line

				string *subString = new string[no_spaces + 1];							// Sub string in each line
				istringstream iss(line);

				i = 0;

			// Tokenizing each line into sub strings
				while (iss && i <= no_spaces)
				{
					string sub;
					iss >> sub;
					subString[i] = sub;
					i++;
				}

				G.gateType = subString[0];
				G.gateNumber = lineCount;

				if (G.gateType != "INPUT" && G.gateType != "OUTPUT")
				{
					G.gateInput1 = atoi(subString[1].c_str());							// Input 1 of the gate

				// Total number of input nodes till gate input 1
					while (node.size() <= G.gateInput1)
						node.push_back(N);

					node.at(G.gateInput1).listofGates.push_back(G);						// List of gates at each node using gate input 1

					// Reading all lines except INV and BUF
					if (G.gateType != "INV" && G.gateType != "BUF")
					{
						G.gateInput2 = atoi(subString[2].c_str());						// Input 2 of the gate
						G.gateOutput = atoi(subString[3].c_str());						// Output of the double-input gate

						// Total number of input nodes till gate input 2
						while (node.size() <= G.gateInput2)
							node.push_back(N);

						node.at(G.gateInput2).listofGates.push_back(G);					// List of gates at each node using gate input 2
					}

					else
						G.gateOutput = atoi(subString[2].c_str());						// Output of the single-input gate

					gate.push_back(G);
				}

				else
				{
					// Reading the INPUTS line
					if (G.gateType == "INPUT")
						for (i = 1 ; i < (no_spaces - 1) ; i++)		                    // Starts from 1 because the 0th subString is the word INPUTS
							PINodes.push_back(atoi(subString[i].c_str()));			    // List of PI nodes

					// Reading the OUTPUTS line
					if (G.gateType == "OUTPUT")
					{
						for (i = 1; i < (no_spaces - 1) ; i++)
						{
							PONodes.push_back(atoi(subString[i].c_str()));				// List of PO nodes
							while (node.size() <= atoi(subString[i].c_str()))
								node.push_back(N);
						}
						break;
					}
				}

				no_spaces = 0;
				lineCount++;
			}

			inputFile.close();
		}
		return 1;
	}

	// Sets all nodes to x
	void setX()
	{
		for (int i = 1; i < node.size(); i++)
		{
			node.at(i).nodeValue = -1;
			node.at(i).nodeFlag  = 0;					// denotes that the value at the corresponding node is x
		}
	}

	// Says whether the PODEM was successful or not!
	int PODEM()
	{
		// Checking if the error is at the Primary Outputs
		for (int i = 0; i < PONodes.size(); i++)
			if (node.at(PONodes.at(i)).faultFlag == 1)
			{
				cout << endl << endl << "SUCCESS";
				cout << endl << endl << "PI nodes" << endl;

				for (int i = 0; i < PINodes.size(); i++)
				{
					cout << PINodes.at(i) << ", ";
				}

				cout << endl << endl << "Input Vector" << endl;

				for (int i = 0; i < PINodes.size(); i++)
				{
					if (node.at(PINodes.at(i)).nodeValue == -1)
						cout << "x";
					else
						cout << node.at(PINodes.at(i)).nodeValue;
				}

				return 1;
			}

		PIBacktrace = Objective();

		// Checking if the D-frontier fails
		if (PIBacktrace.nodeNumber == 0 || PIBacktrace.nodeNumber == -1)
			return 0;

		PISet = Backtrace(PIBacktrace);

		// Checking if the D-frontier fails
		if (PISet.nodeNumber == 0 || PISet.nodeNumber == 0)
			return 0;

		Imply(PISet);
		
		if (PODEM() == 1)
		{
			return 1;
		}

		// Reversing the decision
		PISet.nodeValue = !PISet.nodeValue;
		Imply(PISet);
		
		if (PODEM() == 1)
		{
			cout << endl << endl << "SUCCESS";
			cout << endl << endl << "PI nodes" << endl;

			for (int i = 0; i < PINodes.size(); i++)
			{
				cout << PINodes.at(i) << ", ";
			}

			cout << endl << endl << "Input Vector" << endl;

			for (int i = 0; i < PINodes.size(); i++)
			{
				cout << node.at(PINodes.at(i)).nodeValue;
			}

			return 1;
		}

		PISet.nodeValue = -1;
		Imply(PISet);
		status = 1;

		return 0;
	}

	// Returns the Objective to be achieved
	NodenValue Objective()
	{
		int l, v;
		l = Fault.nodeNumber;
		v = Fault.nodeValue;

		PIObjective.nodeNumber = -1;
		PIObjective.nodeValue = -1;

		// Checking if the fault is activated
		for (int i = 1; i < node.size(); i++)
		{
			if (node.at(i).faultFlag == 1)
			{
				faultActFlag = 1;
			}
		}

		// Checking if the fault node is the Input1 of the gates and if it is x
		if (!faultActFlag)
			for (int i = 0; i < gate.size(); i++)
			{
				if (gate.at(i).gateInput1 == l)
					if (node.at(gate.at(i).gateInput1).nodeValue == -1)
					{
						PIObjective.nodeNumber = gate.at(i).gateInput1;
						PIObjective.nodeValue = !(v);
						return PIObjective;
					}
			}

		// Checking if the fault node is the Input2 of the gates and if it is x
		if (!faultActFlag)
			for (int i = 0; i < gate.size(); i++)
			{
				if (gate.at(i).gateType != "INV" && gate.at(i).gateType != "BUF" && gate.at(i).gateInput2 == l)
					if (node.at(gate.at(i).gateInput2).nodeValue == -1)
					{
						PIObjective.nodeNumber = gate.at(i).gateInput2;
						PIObjective.nodeValue = !(v);
						return PIObjective;
					}
			}

		// Checking if the fault node is the PO node and if it is x
		if (!faultActFlag)
			for (int i = 0; i < PONodes.size(); i++)
			{
				if (PONodes.at(i) == l)
					if (node.at(PONodes.at(i)).nodeValue == -1)
					{
						PIObjective.nodeNumber = PONodes.at(i);
						PIObjective.nodeValue = !(v);
						return PIObjective;
					}
			}

		// Checking for a gate from the D-frontier
		for (int i = 0; i < gate.size(); i++)
		{
			if (gate.at(i).gateType == "INV" || gate.at(i).gateType == "BUF")
			{
				if (isDFrontier(gate.at(i).gateInput1, gate.at(i).gateOutput))
					if (node.at(gate.at(i).gateInput2).nodeFlag == 0)
					{
						PIObjective.nodeNumber = gate.at(i).gateInput2;
						PIObjective.nodeValue = !(ctrlValue(gate.at(i).gateType));
						return PIObjective;
					}
			}
			else if (gate.at(i).gateType == "AND" || gate.at(i).gateType == "OR" || gate.at(i).gateType == "NAND" || gate.at(i).gateType == "NOR")
			{
				if (isDFrontier(gate.at(i).gateInput1, gate.at(i).gateOutput))
					if (node.at(gate.at(i).gateInput2).nodeFlag == 0)
					{
						PIObjective.nodeNumber = gate.at(i).gateInput2;
						PIObjective.nodeValue = !(ctrlValue(gate.at(i).gateType));
						return PIObjective;
					}

				if (isDFrontier(gate.at(i).gateInput2, gate.at(i).gateOutput))
					if (node.at(gate.at(i).gateInput1).nodeFlag == 0)
					{
						PIObjective.nodeNumber = gate.at(i).gateInput1;
						PIObjective.nodeValue = !(ctrlValue(gate.at(i).gateType));
						return PIObjective;
					}
			}
		}
	
		return PIObjective;
	}

	// Maps Objective into PI assignment
	NodenValue Backtrace(NodenValue PIBacktrace)
	{
		int k = PIBacktrace.nodeNumber;
		int vk = PIBacktrace.nodeValue;
		int v = vk;
		int j = 0;
		int inv = 0;
		int flag = 1;

		NodenValue PISetTemp;
		PISetTemp.nodeNumber = -1;
		PISetTemp.nodeValue = -1;

		while (flag)
		{
			flag = 0;
			for (int i = 0; i < gate.size(); i++)
			{
				if (gate.at(i).gateOutput == k)
				{
					inv = invValue(gate.at(i).gateType);
					
					if (node.at(gate.at(i).gateInput1).nodeValue == -1)
						j = gate.at(i).gateInput1;
					else if (gate.at(i).gateType != "INV" && gate.at(i).gateType != "BUF" && node.at(gate.at(i).gateInput2).nodeValue == -1)
						j = gate.at(i).gateInput2;
					
					v = XORValue(v, inv);
					k = j;
					flag = 1;
					break;
				}
			}
		}

		PISetTemp.nodeNumber = k;
		PISetTemp.nodeValue = v;

		return PISetTemp;
	}

	// Propagates the value
	void Imply(NodenValue PIPropagate)
	{
		int nodeNumber = PIPropagate.nodeNumber;
		int nodeValue  = PIPropagate.nodeValue;

		int gin1, gin2, gout;
		string gtype;
		int flag = 1;

		// Set the PI node
		for (int i = 0; i < PINodes.size(); i++)
		{
			int PI = PINodes.at(i);

			if (PI == nodeNumber)
				if (node.at(PI).nodeFlag == 0)
				{
					node.at(PI).nodeValue = nodeValue;
					node.at(PI).nodeFlag = 1;
					if (PI == Fault.nodeNumber)
						node.at(PI).faultFlag = 1;
					break;
				}
		}

		// Evaluate the implications
		while (flag)
		{
			flag = 0;
			for (int i = 0; i < gate.size(); i++)
			{
				gin1  = gate.at(i).gateInput1;
				gin2  = gate.at(i).gateInput2;
				gout  = gate.at(i).gateOutput;
				gtype = gate.at(i).gateType;

				if (node.at(gout).nodeFlag == 0)
				{
					if (node.at(gin1).nodeFlag == 1 && (gtype == "INV" || gtype == "BUF"))
					{
						flag = gate1(gin1, gout, gtype);
					}					
					else if (node.at(gin1).nodeFlag == 1 && node.at(gin1).nodeValue == ctrlValue(gtype) && (gtype == "AND" || gtype == "NAND" || gtype == "OR" || gtype == "NOR"))
					{
						flag = gate2(gin1, gin2, gout, gtype);
					}
					else if (node.at(gin2).nodeFlag == 1 && node.at(gin2).nodeValue == ctrlValue(gtype) && (gtype == "AND" || gtype == "NAND" || gtype == "OR" || gtype == "NOR"))
					{
						flag = gate2(gin1, gin2, gout, gtype);
					}
					else if (node.at(gin1).nodeFlag == 1 && node.at(gin2).nodeFlag == 1 && (gtype == "AND" || gtype == "NAND" || gtype == "OR" || gtype == "NOR"))
					{
						flag = gate2(gin1, gin2, gout, gtype);
					}
				}
			}
		}
	}

	// Evaluate single input gate
	int gate1(int input, int output, string type)
	{
		if (type == "INV")
		{
			node.at(output).nodeValue = !(node.at(input).nodeValue);
			node.at(output).nodeFlag = 1;
			node.at(output).faultFlag = node.at(input).faultFlag;
			if (output == Fault.nodeNumber && (node.at(output).nodeValue != Fault.nodeValue))
				node.at(output).faultFlag = 1;
			return 1;
		}
		else if (type == "BUF")
		{
			node.at(output).nodeValue = node.at(input).nodeValue;
			node.at(output).nodeFlag = 1;
			node.at(output).faultFlag = node.at(input).faultFlag;
			if (output == Fault.nodeNumber && (node.at(output).nodeValue != Fault.nodeValue))
				node.at(output).faultFlag = 1;
			return 1;
		}
		else
			return 0;
	}

	// Evaluate double input gate
	int gate2(int input1, int input2, int output, string type)
	{
		if (type == "AND")
		{
			if (node.at(input1).nodeValue == 0 || node.at(input2).nodeValue == 0)
			{
				node.at(output).nodeValue = 0;
				node.at(output).nodeFlag = 1;
			}
			else
			{
				node.at(output).nodeValue = node.at(input1).nodeValue & node.at(input2).nodeValue;
				node.at(output).nodeFlag = 1;
			}

			faultPropagate(input1, input2, output, type);
			if (output == Fault.nodeNumber && (node.at(output).nodeValue != Fault.nodeValue))
				node.at(output).faultFlag = 1;

			return 1;
		}
		else if (type == "OR")
		{
			if (node.at(input1).nodeValue == 1 || node.at(input2).nodeValue == 1)
			{
				node.at(output).nodeValue = 1;
				node.at(output).nodeFlag = 1;
			}
			else
			{
				node.at(output).nodeValue = node.at(input1).nodeValue | node.at(input2).nodeValue;
				node.at(output).nodeFlag = 1;
			}

			faultPropagate(input1, input2, output, type);
			if (output == Fault.nodeNumber && (node.at(output).nodeValue != Fault.nodeValue))
				node.at(output).faultFlag = 1;

			return 1;
		}
		else if (type == "NAND")
		{
			if (node.at(input1).nodeValue == 0 || node.at(input2).nodeValue == 0)
			{
				node.at(output).nodeValue = 1;
				node.at(output).nodeFlag = 1;
			}
			else
			{
				node.at(output).nodeValue = !(node.at(input1).nodeValue & node.at(input2).nodeValue);
				node.at(output).nodeFlag = 1;
			}

			faultPropagate(input1, input2, output, type);
			if (output == Fault.nodeNumber && (node.at(output).nodeValue != Fault.nodeValue))
				node.at(output).faultFlag = 1;

			return 1;
		}
		else if (type == "NOR")
		{
			if (node.at(input1).nodeValue == 1 || node.at(input2).nodeValue == 1)
			{
				node.at(output).nodeValue = 0;
				node.at(output).nodeFlag = 1;
			}
			else
			{
				node.at(output).nodeValue = !(node.at(input1).nodeValue | node.at(input2).nodeValue);
				node.at(output).nodeFlag = 1;
			}

			faultPropagate(input1, input2, output, type);
			if (output == Fault.nodeNumber && (node.at(output).nodeValue != Fault.nodeValue))
				node.at(output).faultFlag = 1;

			return 1;
		}
		else
			return 0;
	}

	// Propagates the fault
	void faultPropagate(int input1, int input2, int output, string type)
	{
		if (node.at(input1).faultFlag == 1)
			if (node.at(input2).nodeValue == !ctrlValue(type))
				node.at(output).faultFlag = 1;

		if (node.at(input2).faultFlag == 1)
			if (node.at(input1).nodeValue != ctrlValue(type))
				node.at(output).faultFlag = 1;
	}

	// Checks if the gate is a D-frontier
	int isDFrontier(int inputNode, int outputNode)
	{
		if (node.at(inputNode).faultFlag == 1 && node.at(outputNode).nodeFlag == 0)
			return 1;
		return 0;
	}

	// Returns the controlling value of the particular gate
	int ctrlValue(string type)
	{
		if ( type == "AND")
			return 0;
		else if ( type == "NAND")
			return 0;
		else if ( type == "OR")
			return 1;
		else if ( type == "NOR")
			return 1;
		else
			return -1;
	}

	// Returns the inversion value of the particular gate
	int invValue(string type)
	{
		if ( type == "AND")
			return 0;
		else if ( type == "NAND")
			return 1;
		else if ( type == "OR")
			return 0;
		else if ( type == "NOR")
			return 1;
		else if ( type == "INV")
			return 1;
		else if ( type == "BUF")
			return 0;
		else
			return -1;
	}

	// Returns the XOR value
	int XORValue(int a, int b)
	{
		return ((!a & b) + (a & !b));
	}

private:
	
}P_func;

PODEM_functions::PODEM_functions()
{
}

PODEM_functions::~PODEM_functions()
{
}

int main()
{
	int flag = 1;
	int result = 0;
	char ans = 'y';

	flag = P_func.readFile();
	if(flag == 0)
		return 0;

	while (ans == 'y' || ans == 'Y')
	{
		cout << endl << endl << "Enter the faulty node: ";
		cin  >> Fault.nodeNumber;

		cout << "Enter the stuck at value: ";
		cin  >> Fault.nodeValue;

		P_func.setX();
		result = P_func.PODEM();

		if (status == 1)
			cout << endl << endl << "FAILURE";

		status = 0;

		cout << endl << endl << "Do you wish to enter a new fault? : ";
		cin  >> ans;

		for (int i = 1; i < node.size(); i++)
		{
			node.at(i).nodeFlag = 0;
			node.at(i).faultFlag = 0;
		}

		for (int i = 0; i < gate.size(); i++)
		{
			gate.at(i).gateFlag = 0;
		}

		faultActFlag = 0;
	}

	cout << endl << endl;

	system("pause");
	return 0;
}