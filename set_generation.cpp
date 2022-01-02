#include <bits/stdc++.h>

using namespace std;

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
Item* generateItem(std::set<int> subset, std::set<int> indices);

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

// Function to find all subsets of given set. subsets appear only once in the output (due to utilization of std::set)
std::map<string, Item> printPowerSet(int arr[], int n)
{
    vector<string> list;
    std::set<int> items;
    std::map<int, Item> result;
    std::map<string, Item> disjointResult;
    std::set<int> indicesCombined;
    int mapIndex = 0;

    /* Run counter i from 000..0 to 111..1*/
    for (int counter = 0; counter < (int) pow(2, n); counter++)
    {
    	items.clear();
        string subset = "";

        // consider each element in the set
        for (int j = 0; j < n; j++)
        {
            /* Check if jth bit in the counter is set (switched on)
                If so then add jth element from set to subset */

        	// <<	Binary Left Shift Operator. The left operands value is moved left by the number of bits specified by the right operand.
        	// 1 is 2^0 so its on rightmost position it's being shifted by j bits to left every time and the result is bitwise compared with i
        	// by doing this we check the elements from 0 to n-1

        	//Binary & Operator copies a bit to the result if it exists in both operands.
            if ((counter & (1 << j)) != 0)
            {
 //               subset += to_string(arr[j]) + "|";
                items.insert(arr[j]);
            }
        }


//check to assure
        	if(items.size() > 0) {
                int newIndex = mapIndex++;
                std::set<int> subsetSingleIndicesSet; //indices set
                subsetSingleIndicesSet.insert(newIndex); // 0, 1, 2,... sequentially adds the current index (indices, subsets and weights would form a object of Item type)
                Item* item = generateItem(items, subsetSingleIndicesSet); //'items'at this moment is final subset from  0...2^n range; subsetSingleIndicesSet - is subset of its singleton indices

                //we'll have 2 maps. one is 0 2^n subsets data related  called 'result'; 2nd map is for the disjointness of those (disjointResult)
			    string coreKey = generateCombinedKey(items); // 'items' is our elements; coreKey - is space delimited string version of items

			    //is a string indicator of merged set - that allows us to look up in map. exact mechanism below
				//in disjointResult map ; once we found {10}, we put it in results map; as long as it's not in  disjointResult, we do not put it there
				//
				Item* itemFound = new Item(item->subset, item->weight, item->indices); // effectively this means (Item* itemFound = item)
				// but in that case itemFound would have changed 'item' object. this's why that above assignment was done as a workaround
				std::cout << "Before disjointResult  ";
				printData(itemFound);

				std::cout << "is in map " << disjointResult.count(coreKey) <<  " '0'- false, '1'- true " << '\n'; // disjointResult is initialized
				//first time in line 204 below
				// and if there's an item in disjointResult map with that newly generated coreKey from line 132 - we mark it as Found before
				if(disjointResult.count(coreKey) != 0) { //if we are in this clause, means the current key-string map combination was found before
					std::cout << "found Before being found in disjointResult   "; // {10} - 3 , {12}-4 , {10,12} - 5. {10}, {12} they happen to be disjoint; with weight 7; they are put into disjointResult map
					// the newly generated subset {10,12} - 5 apparently has lighter weight than the one already in disjointResult map; the one with heavier weight is taken and
					// the further ops are conducted with it
					//furthermore, e.g. then arrives {14} , then arrives {10, 14} then {12, 14}
					//{10, 14} must be disjoint with {12} ; but say {10,14} has lighter weight than {10}, {14} combined. and we must take that {10}, {14} to try to disjoint it with
					// {12}. confirm with debugging that {10}, {14} is supposed to be in disjointResult map already (with heavier weight)
					printData(&disjointResult[coreKey]);
					// So if it's already Found Before - we check the weight condition
					// if on top of that the weight of new item (matching coreKey) is less that the weight of item from disjointResult with same coreKey, we create itemFound
					if(item->weight < disjointResult[coreKey].weight) { // coreKey is string version of our subset - it is a key to look up in disjoint map;
						//it has one current value during iteration in 0...2^n range



						// one global iteration (ranging from 0 ... 2^n)
						// if the weight of old combinaiton is  < weight of new combination, we create a new item and name it itemFound
						// it matches the subset with larger weight looked up with corresponding coreKey against disjointResult map - e.g. {10}, {14} -from above example
						// Once again , maps contain subsets; we can get string keys named coreKey for each of those subsets {10}, {14} and {10, 14}
						// e.g. merged subset from {10}, {14}  will result in same coreKey as {10, 14} in the disjoint map
						//and that key would point to same subset in disjoint map
						itemFound = new Item(disjointResult[coreKey].subset, disjointResult[coreKey].weight, disjointResult[coreKey].indices);
						// let's continue the example from beginning: {10} then {12}, then {10,12} arrived, etc.
						// generally, our element is now not necessarily from initial result map (e.g. from {10,12} - 5), but we have in disjointResult (the subset weight =7)
						// {10} has not made it to disjointResult map yet (see below); {12} is not there either; this is why it does not make it to this clause
						//	if(disjointResult.count(coreKey) != 0) and we move forward below to iterate thru the results map (which is now non empty)
					}
				}
				// after this last if() clause the 'itemFound' is either the copy of 'item' object or it is the updated one, created on line 159 (depending on meeting the conditions)
				std::cout << "After ";
				printData(itemFound);
				//if 'result' is empty we won't even enter this loop // result is a map with preliminary sets that puts incrementing indices in correspondence with 'item' object
				// {10}, {12}. {10, 12} when those arrive sequentially we add them to 'result' map. their weights are note being changed.
				//first subset e.g. {10} won't make it here b/c/ result map is empty
				for (std::map<int, Item>::iterator it=result.begin(); it!=result.end(); ++it){ // 'result' is initialized down - after for() loop ends
					if(is_disjoint(items, ((Item)it->second).subset)) {// second subset, e.g. {12} (which is current 'items') ; it's convenient to use 'items' for is_disjoint instead of 'ItemFound'
						// will be checked if it's disjoint with (Item)it->second).subset)-each subsequent element of 'results' map. In this case its only first element , i.e. {10}
					    std::set<int> subsetCombined(((Item)it->second).subset); // now, if we are in this if() clause, we must merge {10}, {12}

					    subsetCombined.insert(itemFound->subset.begin(), itemFound->subset.end()); // in the state described so far the itemFound is not yet initialized in
					    //line 159, but rather it takes the value of 'item' object from line 130. i.e. it's currently corresponds to {12} subset & its associates
					    //
					    string combinedKey = generateCombinedKey(subsetCombined); // gets the string version of unique key pointing to the combined subset
					    // 10 12 string is the key for disjointResult map
		                std::cout << combinedKey << " => " << itemFound->weight + ((Item)it->second).weight << "   "; //  weights of constituents are added

		                //idea is to iterate through
		                indicesCombined.clear();
		                indicesCombined.insert(it->first); //single
		                indicesCombined.insert(itemFound->indices.begin(), itemFound->indices.end()); // may be more than 1 index

					    for (int subsetVal : indicesCombined) {
							std::cout << subsetVal << " ";
						}
						std::cout << '\n';
						// subsetCombined is {10. 12} after 12 arrived;
						// the weights of elements above is added, the indices subsets indicesCombined, subsets are combined too in subsetCombined
						//((Item)it->second).weight - type 'item'- in this case weight of {10} is taken; itemFound->weight weight of item {12} is taken
						Item* combinedItem = new Item(subsetCombined, itemFound->weight + ((Item)it->second).weight, indicesCombined);
						if(disjointResult.count(combinedKey) == 0 || disjointResult[combinedKey].weight < combinedItem->weight) {// if the item with 'combinedKey' did not exist - fill
							disjointResult[combinedKey] = *combinedItem; //(i,e. with '10 12' string key), or IF weight of existing element in disjointResult map is lighter
							// key remains the same in map, but the value is updated with element of type 'item' (indices are 0 and 1, subset is merged, weights added)
							// in continuation of our example, now disjointResult is non empty; so when we're on next iteration to generate next subset (from 0..2^n range)
							// disjointResult is not empty anymore

							//if new item is not intersecting with anyone - a combined is created in two cases: whether the combined was not thre or the weight condition above is satisfied
						}
					}
					else {// here a check with coreKey is done in case if Iterable set from result map is NOT disjoint with items
						if(disjointResult.count(coreKey) == 0 || disjointResult[coreKey].weight < item->weight) {
							disjointResult[coreKey] = *item; //
							// coreKey was generated to check if we had such an element; we fill the disjointResult either if it is not there
							// or in case if it is there and its weight is less than the weight of the item ; disjointResult is updated with item object
							// this is reverse of operation we've done above.
							// so if not disjoint, an item's coreKey is looked up in disjointResult (e.g. '10 12' is string key for newly arrived {10,12}).
							// But there was already same ('10 12') added as combined string key as a result of merge of {10} and {12}
							// if it happens to be that {10} with weight '1' , {12} - '1' and  separately  {10,12} has weight '4' ;
							// so object of type items : {10,12} is placed disjointResult with coreKey (with value '10 12')
						}

					}
                } // result is the "simple" mapping of subset , weights and indices; it' s generated every time an increment of 'mapIndex' happens (which runs along 'counter' in range 0..2^n)
                result[newIndex] = *item; // indices in result map are simple; indices in disjoint map can be > 1 and up to n
        	}
        }

    	std::cout << "\n\n\n";
		for (std::map<int, Item>::iterator it=result.begin(); it!=result.end(); ++it){
				std::cout << it->first << "  => " << ((Item)it->second).weight << " => " << ((Item)it->second).subset.size() << " => ";

				for (int subsetVal : ((Item)it->second).subset) {// second section of output from above is filled here;
					std::cout << subsetVal << " ";
				}
				std::cout << '\n';
		}

		return disjointResult; //this candidates mapping is returned as a result of running in 0 - 2^n range
    }


Item* generateItem(std::set<int> subset, std::set<int> indices)
{
	// some random number in given range is generated
	int weight = (rand()%600)+1;

	Item* item = new Item(subset, weight, indices);
	return item;
}

// Driver code
int main()
{
	Item *finalItem = NULL;
	srand((unsigned)time(0));

    int arr[] = { 10, 12, 14 };
    int n = sizeof(arr)/sizeof(arr[0]);


    std::map<string, Item> mapResult = printPowerSet(arr, n);

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

