#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>



using namespace std;


//this is single class that we have. it has 3 fields - a) indices b) subset of integers and b) weight of that subset
class Item {
public:
	std::set<int> subset;
	std::set<int> indices;
	int weight;

public:
	Item (std::set<int> subset, int weight, std::set<int> indices);
    Item() {
        this->weight = 0;
    };
    Item(const Item& item) {
        this->indices = item.indices;
        this->subset = item.subset;
        this->weight = item.weight;
    };
};



Item::Item (std::set<int> subset, int weight, std::set<int> indices)
{
	//initialization of member fields
	this->indices = indices;
	this->subset = subset;
	this->weight = weight;
};

// declaration of generateItem f-n
Item* generateItem(std::set<int> subset, int weight , std::set<int> indices);



string generateCombinedKey(const std::set<int> &mergedSet) {
	string combinedKey = "";
	for (int subsetVal : mergedSet) { //sa inch a anum ?
		if (combinedKey != "") {
			combinedKey.append(" ");
		}
		combinedKey.append(std::to_string(subsetVal));
	}
	return combinedKey;
}

void printData(Item *itemFound) {
	std::cout <<  " weight "<< itemFound->weight << " => ";
	for (int subsetVal : itemFound->subset) {//subset can be result of merge as well
		std::cout << subsetVal << " ";
	}
	std::cout << "subset(s) with index(ices) ";
	for (int subsetVal : itemFound->indices) {
		std::cout << subsetVal << " ";
	}
	std::cout << '\n';
}



template<class Set1, class Set2>
bool is_disjoint(const Set1 &set1, const Set2 &set2)
{
    if(set1.empty() || set2.empty()) return true;

    typename Set1::const_iterator
        it1 = set1.begin(),
        it1End = set1.end();
    typename Set2::const_iterator
        it2 = set2.begin(),
        it2End = set2.end();

    if(*it1 > *set2.rbegin() || *it2 > *set1.rbegin()) return true;

    while(it1 != it1End && it2 != it2End)
    {
        if(*it1 == *it2) return false;
        if(*it1 < *it2) { it1++; }
        else { it2++; }
    }

    return true;
}

std::map<string, Item> readCSVFile()
{
    //std::map<int, std::set<int>> result;


    vector<string> list;
    std::set<int> items;
    std::map<string, Item> result;
    std::map<string, Item> disjointResult;
    std::set<int> indicesCombined;
    int mapIndex = 0;
	//ofstream myfile2;
	//myfile2.open ("example.csv");
    std::ofstream myfile1;
    myfile1.open ("weights.json");

    auto file_path = "instance_ex1.txt";
    std::ifstream infile(file_path);

    string line;
    istringstream strm;

    int num;
    bool skip_line = true;
    while (getline(infile, line, '\r'))
    {
    	items.clear();
    	string subset = "";
        if (skip_line)
        {
            skip_line = false; //takes care of top row - which is to be skipped
            continue;
        }

        istringstream strm(line);

        std::set<int> unique_ints;  // will be commented out soon
        int first_pos_val = INT_MIN;
        while (strm >> num)
        {

            if (first_pos_val == INT_MIN)
            {
                first_pos_val = num;
                continue;
            }
            items.insert(num); // porting to previous f-n analogue of items.insert(arr[j]); in previous implementaiton
        }

                	if(items.size() > 0) {
                        int newIndex = mapIndex++;
                        std::set<int> subsetSingleIndicesSet; //indices set
                        subsetSingleIndicesSet.insert(newIndex); // 0, 1, 2,... sequentially adds the current index (indices, subsets and weights would form an object of Item type)

                       	Item* item = generateItem(items, first_pos_val , subsetSingleIndicesSet); //'items'at this moment is final subset from  0...2^n range; subsetSingleIndicesSet - is subset of its singleton indices


        			    string coreKey = generateCombinedKey(items); // 'items' is our elements; coreKey - is space delimited string version of items

        				Item* itemFound = new Item(item->subset, item->weight, item->indices); // effectively this means (Item* itemFound = item)
        				std::cout << "Before disjointResult  ";
        				printData(itemFound);

        				std::cout << "is in map " << disjointResult.count(coreKey) <<  " '0'- false, '1'- true " << '\n'; // disjointResult is initialized
        				if(disjointResult.count(coreKey) != 0) { //if we are in this clause, means the current key-string map combination was found before
        					std::cout << "found Before being found in disjointResult   "; // {10} - 3 , {12}-4 , {10,12} - 5. {10}, {12} they happen to be disjoint; with weight 7; they are put into disjointResult map
        					printData(&disjointResult[coreKey]);
        					if(item->weight < disjointResult[coreKey].weight) { // coreKey is string version of our subset - it is a key to look up in disjoint map;
        						itemFound = new Item(disjointResult[coreKey].subset, disjointResult[coreKey].weight, disjointResult[coreKey].indices);
        					}
        				}
        				std::cout << "After ";
        				printData(itemFound);
        				for (std::map<string, Item>::iterator it=disjointResult.begin(); it!=disjointResult.end(); ++it){ // 'result' is initialized down - after for() loop ends
        					if(is_disjoint(items, ((Item)it->second).subset)) {// second subset, e.g. {12} (which is current 'items') ; it's convenient to use 'items' for is_disjoint instead of 'ItemFound'
        					    std::set<int> subsetCombined(((Item)it->second).subset); // now, if we are in this if() clause, we must merge {10}, {12}

        					    subsetCombined.insert(itemFound->subset.begin(), itemFound->subset.end()); // in the state described so far the itemFound is not yet initialized in
        					    string combinedKey = generateCombinedKey(subsetCombined); // gets the string version of unique key pointing to the combined subset
        		                std::cout << combinedKey << " => " << itemFound->weight + ((Item)it->second).weight << "   "; //  weights of constituents are added

        		                std::set<int> indicesCombined(((Item)it->second).indices); //single, b/c coming out from result map
        		                indicesCombined.insert(itemFound->indices.begin(), itemFound->indices.end()); // may be more than 1 index

        					    for (int subsetVal : indicesCombined) {
        							std::cout << subsetVal << " ";
        						}
        						std::cout << '\n';
        						Item* combinedItem = new Item(subsetCombined, itemFound->weight + ((Item)it->second).weight, indicesCombined);
        						if(disjointResult.count(combinedKey) == 0 || disjointResult[combinedKey].weight < combinedItem->weight) {// if the item with 'combinedKey' did not exist - fill
        							disjointResult[combinedKey] = *combinedItem; //(i,e. with '10 12' string key), or IF weight of existing element in disjointResult map is lighter
        						}
        					}
                        }
                        result[std::to_string(newIndex)] = *item;
						if(disjointResult.count(coreKey) == 0 || disjointResult[coreKey].weight < item->weight) {
							disjointResult[coreKey] = *item; //
						}

                	}
    }


    	std::cout << "\n\n\n";




    	bool isFirst = true;
    	myfile1 << '[';
		for (std::map<string, Item>::iterator it=result.begin(); it!=result.end(); ++it){
				std::cout << it->first << "  => " << ((Item)it->second).weight << " => " << ((Item)it->second).subset.size() << " => ";

	//			std::cout << std::setw(2) << object << '\n'; // temporary commented
				if(!isFirst) {
			    	myfile1 << ",";
				}
				else {
					isFirst = false;
				}
		    	myfile1 << "{\"s\":[";
		    	bool isFirstInArray = true;

				for (int subsetVal : ((Item)it->second).subset) {// second section of output from above is filled here;
					if(!isFirstInArray) {
				    	myfile1 << ",";
					}
					else {
						isFirstInArray = false;
					}
			    	myfile1 << subsetVal;



					std::cout << subsetVal << " ";
				}
		    	myfile1 << "],";

				myfile1 << "\"w\":" << ((Item)it->second).weight << "}";

				std::cout << '\n';

		}

    	myfile1 << ']';


		myfile1.close();

		return disjointResult; //this candidates mapping is returned as a result of running through range of subsets given in problem instance

}



Item* generateItem(std::set<int> subset, int weight ,std::set<int> indices)
{

	//weights used to be random in 1st implementation , now they are read from problem instance
	Item* item = new Item(subset, weight, indices);
	return item;
}


 // Driver code
int main()
{


	Item *finalItem = NULL;
	srand((unsigned)time(0));

    std::map<string, Item> mapResult = readCSVFile();

	std::cout << "\n\n\n";
	std::cout << "Disjoint Set candidates map is returned";
	std::cout << "\n";
	for (std::map<string, Item>::iterator it=mapResult.begin(); it!=mapResult.end(); ++it){
		Item *item = &(it->second);
		printData(item);
		if(finalItem == NULL) {
			finalItem = item;
		}
		if (finalItem->weight < item->weight) {
			finalItem = item; //outputs the combined item with largest weight
		}
	}
	std::cout << "\n\n\n\n";
	std::cout << "the Winner is determined";
	std::cout << "\n";
	printData(finalItem);

    return 0;
}

