#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <functional>
#include <algorithm> 
using namespace std;

struct meta_frame{
		
	//object to store all input information
    vector<unsigned int> control_index;
    vector<vector<string>> probe_id;
    vector<string> sample_id;
    vector<vector<float>> data_frame;

};

vector<string> get_control_probes(char* input_file){
    
    //gets line delimited list of probe ID stores in iteratable string vector
    ifstream infile;
    vector<string> pusher;
    infile.open(input_file);
    while (!infile.eof()){
        string sLine = "";
        getline(infile,sLine);
        pusher.push_back(sLine);
    }
    infile.close();
	return pusher;
	
}

meta_frame get_data(char* input_file, vector<string> control_probes){
    
	//opens file with log2 signal intensities and stores in struct object
    int line_count = 0;
	meta_frame pusher;
    ifstream infile;
    infile.open(input_file);
    while (!infile.eof()){
		//opens the file, and iterates and tokenizes lines
        string sLine, token;
        vector<string> string_holder;
        getline(infile,sLine);
        istringstream iss(sLine);
        while(getline(iss,token,'\t'))
            string_holder.push_back(token);
        //stores headers, i.e., sample id
        if (line_count == 0){
            for (x : string_holder)
                pusher.sample_id.push_back(x);
        }else{
			//stores probe identifiers, and floats data
			if (string_holder.size() != 0){
				vector<string> holder;
				vector<float> float_push;
				holder.push_back(string_holder[0]);
				holder.push_back(string_holder[1]);
				pusher.probe_id.push_back(holder); 
				for (int x = 2; x < string_holder.size(); x++)
					float_push.push_back(std::stof(string_holder[x]));
				pusher.data_frame.push_back(float_push);         
				//checks if probe id for the current line is a control probe
				//if yes, stores the 0 index location of probe in the data matrix
				for (x : control_probes){
					if (x == string_holder[0]){
						pusher.control_index.push_back(line_count - 1);
					}
				}
			}
   		}
        line_count++;
    }
    infile.close();
    return pusher;
}

vector<float> passfilter(meta_frame &input_data){
    
    //determines the passing filter for each sample
    vector<float> filter; 
    //for each sample collects the control probe signal intensities then calculates 95 percentile
    //stores result in vector 0-indexed by sample id
    for (int y = 0; y < input_data.data_frame.at(0).size(); y++){
        vector<float> holder;
        for (z : input_data.control_index)
            holder.push_back(input_data.data_frame[z][y]);
        float min = *std::min_element(holder.begin(), holder.end());
        float max = *std::max_element(holder.begin(), holder.end());
        filter.push_back((0.95 * (max - min)) + min);
    }
	return filter;
	
}

void print_data(meta_frame &input_data, vector<float> filter){
    
    //prints headers
    for (x : input_data.sample_id)
		cout << x << "\t";
	cout << "\n";
    
    //iterates through rows of data and determines if each data point passes for each respective sample filter 
    for (int x = 0; x < input_data.data_frame.size(); x++){
		//prints probe information
		cout << input_data.probe_id[x][0] << "\t" << input_data.probe_id[x][1] << "\t";
		for (int y = 0; y < input_data.data_frame.at(x).size(); y++){
			//if greater than filter
			if (input_data.data_frame[x][y] > filter[y]){		
				//if greater than filter by more than 1
				if (input_data.data_frame[x][y] > (filter[y] + 1))
                    cout << "P" << "\t";
                //if only marginally greater
                else
                    cout << "M" << "\t";
            }else //if not greater than filter
                cout << "F" << "\t";
		}
		cout << "\n";
	}
}

int main(int argc, char* argv[]){
	
	//usage ./microarray_filter log2_signal_intensities.txt control_probe_list.txt
	
	//load control probes
    vector<string> control_list = get_control_probes(argv[2]);
    
    //import data
    meta_frame data = get_data(argv[1], control_list);
    
    //determine pass filter for each sample
    vector<float> filter = passfilter(std::ref(data));
    
    //output results to STOUT
    print_data(std::ref(data), filter);
    
	return 0;

}
