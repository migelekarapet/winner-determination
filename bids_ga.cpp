// this VERSION contains alternative crossover and mutation f-n improved: 04.04.22 - needs DEBUG
// heavily changed no_changes related lopps below : 05.05.22 - needs DEBUG

#include <iostream>
#include <bits/stdc++.h>

// a fast genetic algorithm for the 0-1 winner det. problem
// test case: 1000 items, 50 knapsack size
//
// compilation by: g++ genetic.cpp -O3 -ffast-math -fopenmp
#include <math.h>
#include <time.h>
#include <algorithm>
#include <limits.h>
#include <numeric>

//#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <set>



using namespace std::chrono;



using namespace std;


#if defined(__linux) || defined(__linux__)
unsigned int seed = time(NULL);
#define RND ((double)rand_r(&seed)/RAND_MAX) // reentrant uniform rnd
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define RND ((double)rand()/RAND_MAX) // uniform rnd
#endif

using namespace std;




template<class Set1, class Set2>
bool is_disjoint(const Set1& set1, const Set2& set2)
{
	if (set1.empty() || set2.empty()) return true;

	typename Set1::const_iterator
		it1 = set1.begin(),
		it1End = set1.end();
	typename Set2::const_iterator
		it2 = set2.begin(),
		it2End = set2.end();

	if (*it1 > *set2.rbegin() || *it2 > *set1.rbegin()) return true;

	while (it1 != it1End && it2 != it2End)
	{
		if (*it1 == *it2) return false;
		if (*it1 < *it2) { it1++; }
		else { it2++; }
	}

	return true;
}






template<typename T>
void set_new_lambda(std::exponential_distribution<T> *exp_dis, T val)
{
    typename std::exponential_distribution<T>::param_type new_lambda(val);
    exp_dis->param(new_lambda);
}



int clash_magnitude_vector_sets(const vector<std::set<int>>& selected_items)
{
	// check here how many of subsets are clashing// advanced: ideally scale of clashing also
	if (selected_items.empty()) return 0;

	int n_sel_items = selected_items.size();
	int n_magnitude = 0;
	std::set<int> my_set;
	set<int>::iterator itr;

	for (int i = 0; i < n_sel_items; i++)
	{
		for (int j = i + 1; j < n_sel_items; j++)
		{
			if (is_disjoint(selected_items[i], selected_items[j])) { continue; }
			else { n_magnitude += 1; }
		}
	}

	return n_magnitude;
}

double total_w_all_subsets(const vector<double>& w)
{

	int w_size = w.size();
	if (w_size == 0) return 0;

	double n_weightTotal = 0.0;
	//std::set<int> my_set;
	//set<int>::iterator itr;

	for (int i = 0; i < w_size; i++)
	{
		n_weightTotal += w[i];
	}

	return n_weightTotal;
}

bool is_disjoint_vector_sets(const vector<std::set<int>>& selected_items)
{
	// check here how many of subsets are clashing// advanced: ideally scale of clashing also
	if (selected_items.empty()) return true;

	int n_sel_items = selected_items.size();
	int act_l_length = 0;
	std::set<int> my_set;
	set<int>::iterator itr;

	for (int i = 0; i < n_sel_items; i++)
	{
		for (itr = selected_items[i].begin(); itr != selected_items[i].end(); itr++)
		{
			my_set.insert(*itr);
			act_l_length += 1;
		}
	}

	return (my_set.size() == act_l_length);
}


struct chromo {
	chromo(int dimc = 0) {
		if (dimc == 0)
			items = NULL;
		else
			items = new bool[dimc];
		item_count = dimc;
	}
	~chromo() { items = NULL; }
	void mutate(const int dimc, const int count) {
		int mi;
		for (int i = 0; i <= count; i++) {
			mi = (int)(round((double)rand() / RAND_MAX * (dimc-1)));
			items[mi] = !items[mi];
		}
	}
	bool* items;
	double f{ 0 };
	int item_count{ 0 };
};

template<typename T>
double getAverage(std::vector<T> const& v) {
    if (v.empty()) {
        return 0;
    }
    return std::accumulate(v.begin(), v.end(), 0.0) / v.size();
}


template<typename T>
std::vector<T> create_copy(std::vector<T> const& vec)
{
	std::vector<T> v(vec);
	return v;
}

void create_ch_copy(const std::vector<chromo>& vecSrc, std::vector<chromo>& vecDest)
{
	const size_t count = vecSrc.size();
	vecDest.resize(count);
	for (size_t i = 0; i < count; ++i) {
		vecDest[i].f = vecSrc[i].f;

		int item_cout = vecSrc[i].item_count;
		vecDest[i].item_count = item_cout;
		vecDest[i].items = new bool[item_cout];
		for (int j = 0; j < item_cout; ++j)
			vecDest[i].items[j] = vecSrc[i].items[j];
	}
}

void clear_ch(std::vector<chromo>& vec)
{
	const size_t count = vec.size();
	for (size_t i = 0; i < count; ++i) {
		int item_cout = 0;
		delete[] vec[i].items;
	}

	vec.clear();
}

double fitness(bool*& x, const int dim_chromo, const vector<std::set<int>>& array_subsets, const vector<double>& w, int range) 
{
	// IN this context x - is the 0, 1 combination of bits that would light up (or not) the given subset
	// we should multiply

	double fit = 0.0;
	bool feasible = true;


	//vector<std::set<int>> selected_items;

	//int* inside_count = new int[range];
	int inside_count[1000] = { };

	for (int i = 0; i < dim_chromo; i++) {
		if (x[i] == 0) { continue; } // '0' means subset did not light up

		//selected_items.push_back(array_subsets[i]);
		set<int>::iterator itr;

		for (itr = array_subsets[i].begin(); itr != array_subsets[i].end(); itr++)
		{
			if (inside_count[(*itr) - 1] >= 1) //DEBUG ! (-1 momente ) array_subsets[i][j] is representing the integers in range; if it was already included this clause is satisfied
			{
				feasible = false;
				//break;
			}

				  
			inside_count[(*itr) - 1] += 1;
		}

		if (feasible)
			fit += x[i] * w[i]; //c[i] per se has been passed as referent under name x; now, x[i] is the element of that bool[dimw] array. i.e. it cromosome's position indic, 0 or 1
	}

	if (feasible)
	{ 
		//delete[] inside_count;
		return fit; // no clashes observed
	}


	else
	{
		double magnitude = 0.0;
		for (int k = 0; k < range; k++)
		{
			// when we see 0 , that item is not included
			// when we see 1 - it was selected by 1 bid - which is fine
			if (inside_count[k] == 0 || inside_count[k] == 1) continue;

			// when we see >=2 that item was selected by multiple bids - in this case we add that number of (counts - 1) to the magnitude value (which is initially 0)
			else
				magnitude += (double)(inside_count[k]);
			// above we go through all the items and determine magnitude for each repeated one; this is even more precise calculation than in previous clash_magnitude_vector_sets() which dealt with count of set clashes only
		}

		
		double w_aggregate = total_w_all_subsets(w);
		double scaled_factor = 10*(fit / w_aggregate);
		fit = fit - fit * (magnitude + scaled_factor);
		//fit = -(INT_MAX);//reconsider
		//delete[] inside_count;
		return fit;
	}

}
int random_int(int min, int max)
{	
  return (int)(round(((double)rand() / RAND_MAX) * (max - min)));
}


void crossover1p(const chromo& c1, const chromo& c2, const chromo& c3, const int dimc, const int cp) {
	for (int i = 0; i < dimc; i++) {
		if (i < cp) { c3.items[i] = c1.items[i]; } //if i<cp - looks crazy ! - m.k.
		else { c3.items[i] = c2.items[i]; }
	}
}

void crossover1p_sw(const chromo& c1, const chromo& c2, const chromo& c3, const int dimc) {
	int pos = random_int(0, dimc-1);

	// 
	// adopt only copying from copy_ch

	for (int k = 0; k < dimc; ++k)
	{					
		if (k <= pos && k!= dimc-1) // Case 1. to ensure at least one bit from c1 is copied (case pos ==0); 
			c3.items[k] = c1.items[k]; // second clause (... && k!= dimc-1) enables the Case 2 below
		else
		{
			c3.items[k] = c2.items[k]; // Case 2. to ensure at least one bit from c2 is copied (when pos == dimc-1)
		}
	}

	return;
}



double localsearch(const chromo& curr_best, const int dim_chromo, const vector<std::set<int>>& array_subsets,
		const vector<double>& w, double curr_best_val, int range) {
	//bool is_improved = true;
	int curr_best_ind = INT_MAX;
	double best_val = curr_best_val;
	chromo copy_input_best = curr_best; // keeping a copy of curr_best which was passed as a reference

//	while(is_improved)
//	{
		for (int j = 0; j < dim_chromo; j++) {

			copy_input_best.items[j] = !copy_input_best.items[j]; // inverting one bit of chromosome

			double curr_fit = fitness(copy_input_best.items, dim_chromo, array_subsets, w, range);

			copy_input_best.items[j] = !copy_input_best.items[j]; // reverting back the negation of chromosome's bit

			if (curr_fit > best_val)
			{
				best_val = curr_fit;
				curr_best_ind = j;
				//is_improved = true;

			}
		}

		if (best_val > curr_best_val)
		{
			// modifying the chromosome (returned as reference) and positioning it at local max
			curr_best.items[curr_best_ind] = !curr_best.items[curr_best_ind];
		}
//	}
	return best_val;
}



bool cfit(const chromo& c1, const chromo& c2) { return c1.f > c2.f; }
bool cmpfun(const std::pair<int, double>& r1, const std::pair<int, double>& r2) { return r1.second > r2.second; }

int coin(const double event_prob) { // a cointoss
	if ((double)rand() / RAND_MAX <= event_prob) return 1; // first we tried return 1 is crossover o/w it's mutation
	else return 0; // then mutation prob was lowered to 0.03 and we just call this function for mutaiton with its prob
}

// initializes the chromosomes
void initpopg(bool**& c, const std::vector<double>& w, const int dimw, const int pop)
{
	for (int i = 0; i < pop; i++) {
		for (int j = 0; j < dimw; j++) {
			c[i][j] = (int)rand() % 2;
		}
	}
}
/*
enum class string_code {
		in404_100,
		in102_100,
		in400
	};


std::map<std::string, string_code> s_mapStringToTestType =
{
    { "in404_100.txt", string_code::in404_100 },
    { "in102_100.txt", string_code::in102_100 },
	{ "in400.txt", string_code::in400 }
};
*/



int main() {
	printf("\n");
	srand((unsigned)time(NULL));
	vector<double> w; // items weights and values
	vector<double> best_arr;


	std::set<int> subsets;
	vector<std::set<int>> array_subsets;

	std::ofstream myfile1, myfile2;


	//double global_extrema = 0.0;



//	for (auto s : vector<string>{"in404_100.txt", "in102_100.txt", "in400.txt"} )
//	{
//		auto file_path = s;
		auto file_path = "in404_175.txt";
		myfile1.open("cross_mut_found_in404_175.csv");
		myfile2.open("cross_mut_avg_in404_175.csv");
		int range = 1000;


/*		string_code stringCode = s_mapStringToTestType[(std::string)file_path];
		switch (stringCode)
		{
		    case string_code::in404_100:
		    	global_extrema = int(51310.243);
		        break;

		    case string_code::in102_100:
		    	global_extrema = int(37609);
		        break;

		    case string_code::in400:
		    	global_extrema = int(53251.2);
		        break;

		    default:
		        printf("Unknown TestType ");
		}*/

	// create loop to parse each of elements of that array = to open one file a t a time
	std::ifstream infile(file_path);

	string line;
	istringstream strm;

	double num;
	getline(infile, line); // top row to be skipped
	while (getline(infile, line))
	{
		subsets.clear();

		istringstream strm(line);

		//std::set<int> unique_ints;  // will be commented out soon
		double first_pos_val = (-DBL_MAX);
		while (strm >> num)
		{

			if (first_pos_val == (-DBL_MAX))
			{
				first_pos_val = num;
				continue;
			}
			subsets.insert(int(num)); // porting to previous f-n analogue of items.insert(arr[j]); in previous implementaiton
		}

		w.push_back(first_pos_val);
		array_subsets.push_back(subsets);

	}
	/*
	 * for instance with bid size = 100, the GA+local search found the solution in approx 1 hour with => settings: pop = 100;  disc = (int)(ceil(pop * 0.98)) 	double cross_prob = 0.9; // crossover and mutation probabilities
	double mut_prob = 0.5; mutation happens in else clause after crossover. i.e. initially 10% mutaiton rate is set, then when mut_prob = 0.5 is applied it drops further to 5%

	Case 2: same settings as above, but mutation rate mut_prob = 0.15 is applied it drops further to 1.5% - did not converge in 1 hour

	 * */

	const int pop = 300; // chromosome population size
	const int gens = 100000; // maximum number of generations - switch back to 1000000
	const int disc = (int)(ceil(pop * 0.98)); // chromosomes discarded via elitism
	const int dimw = w.size(); // dimensions of chromosome
	double best = 0.0;
	bool b_loc_search = false;
	int ind = 0, ind2 = 0; // a few helpers for the main()
	//int parc = 0; // parent index for crossover
	int parc = random_int(0, pop - 1);
	//double cross_prob = 0.9; // crossover and mutation probabilities
	//double mut_prob = 1;




	//double avg = 0;
	vector<chromo> ch(pop, chromo(dimw)), ch_copy(pop, chromo(dimw));// vector chromo which is defined via size and type of elements
	bool** c = new bool* [pop];
	for (int i = 0; i < pop; i++) c[i] = new bool[dimw];
	clock_t start = clock();
	printf("Initializing population ...");
	// simplify init pop gen with simple rand chromosomes generation
	initpopg(c, w, dimw, pop); // peredacha po ssilke array of arrays 'c'
	printf("done!");
	for (int i = 0; i < pop; i++) { // ch[i] - is ith chromosome; which has properties 'f' - fitness; items - sequence of '0's and '1's
		ch[i].items = c[i]; //c[i] = new bool[dimw]; initialized few lines above in initpopg; and here we initialize 'items' member of chromosome class (or struct)
		ch[i].f = fitness(ch[i].items, dimw, array_subsets, w, range); // chromosome has a member 'f' indicating its fitness
	}
	printf("\n\n");
	//create_ch_copy(ch_copy, ch); uncomment when mut / pop loop is enabled


	double cross_prob = 1.0; // crossover and mutation probabilities
	double mut_prob = 0.2;
	bool no_changes=false;

	//int mutation_cycle = 0;
	//int cross_cycle = 0;
	//take out the mutation and crossover params. place them in loops ; step size = 0.05
	//calculate average by the end ...
/*	for (int c = 50; c <= 100; ++c) // as of cross_prob = 0.05 to cross_prob = 1 with step 0.05
	{

		if (c % 10 == 0)
		{
			cross_prob = c / 100.0;
			no_changes = false;
		}
		else { continue; }

		for (int m = 50; m >= 0; --m) // m = 1 to 0 step 0.05
		{

			if (m % 10 == 0)
			{
				mut_prob = m / 100.0;
				no_changes = false;
			}
			else { continue; }*/

			printf("Starting new gen cycle with  mut_prob equal to");
			printf("\n#%f\t", mut_prob);
			printf("\n");
			printf("And cross_prob equal to");
			printf("\n#%f\t", cross_prob);
			printf("\n");


			/*if (ch.size() > 0) //needed only when mut/pop loops are present
			{
				clear_ch(ch);
				create_ch_copy(ch_copy, ch);
				printf("\n\n");
			}*/


			if (best_arr.size() > 0)
				best_arr.clear(); //clearing the best array generated for given combination of crossover and mutation rate

			for (int p = 0; p < gens; p++)
			{
				std::sort(ch.begin(), ch.end(), cfit); // sorts by fitness of chromosomes

#pragma omp parallel for shared(ch) //mb comment out ?

				best = 0; // added in the evening RETHINK
				for (int i = 0; i < pop; i++)
				{
					if (i > pop - disc)
						{ // elitism - only processes the discarded chromosomes
						
							if (coin(cross_prob) == 1)
								{ // crossover section
									ind = parc + disc + round(10 * (double)rand() / RAND_MAX); // choosing parents for crossover - RETHINK !!!
									ind2 = parc + disc + 1 + round(10 * (double)rand() / RAND_MAX); //  RETHINK !!!
									//ind = random_int(0, pop - 1);
									//ind2 = random_int(0, pop - 1);
									int tmp1 = ind % pop;
									int tmp2 = ind2 % pop;



									// choose a crossover strategy here
									//crossover1p(ch[ind % pop], ch[ind2 % pop], ch[i], dimw, round((double)rand() / RAND_MAX * (dimw - 1)));
									crossover1p_sw(ch[ind % pop], ch[ind2 % pop], ch[i], dimw); //DEBUG 04.05.22
									//crossover1p_sw(ch[ind], ch[ind2], ch[i], dimw); //DEBUG 04.05.22

									//int pos = random_int(1, pop-1); // experim 05.05
									//crossover1p_sw(ch[pos], ch[pop-pos], ch[i], dimw);  ////DEBUG 05.05.22


									ch[i].f = fitness(ch[i].items, dimw, array_subsets, w, range);
									parc += 1;
								}
							//							else { // mutation section
							if (coin(mut_prob) == 1)
							{
								ch[i].mutate(dimw, 1);
								ch[i].f = fitness(ch[i].items, dimw, array_subsets, w, range);
							}
						//							}
						}

					//avg += ch[i].f;
					if (ch[i].f > best)
					{
						best = ch[i].f;

						//if(b_loc_search) //still set to false - modified the calling part below so this is not called in this version 05.05
						//{

							double next_b = localsearch(ch[i], dimw,  array_subsets, w, best, range);
							// note: here ith index of ch[i] indicates ith chromo; and its items' indices - e.g. ch[0].items[0] ,..., ch[0].items[6] indicate 0 or 1 depending on which subset is lit

							// best in the beginning is taken from sorted array or chrome
							while (next_b > best)
							{
								// next_b represents current best improvement after local search carried out.
								// if next_b was improved already accepted (modified) ch[i] is passed in its best form again to local search
								best = next_b;
								next_b = localsearch(ch[i], dimw,  array_subsets, w, best, range);
							}
						//}
					}


				} // when this endsloop  means we ran over one generation

				parc = 0;

				best_arr.push_back(best); //best of one pupulation run is pushed into best_arr

				if (p % 5 == 0) {
					printf("\n#%d\t", p);
					printf("best fitness: %f \t", best);

				}

				if (best <=   61305.2      + 0.1 && best >=   61305.2   - 0.1)
				{
					myfile1 << best;
					myfile1 << "\n";
					myfile1 << cross_prob;
					myfile1 << ",";
					myfile1 << mut_prob;
					myfile1 << "\n";

					clock_t end = clock();
					double t = (double)(end - start) / CLOCKS_PER_SEC;
					printf("\nCompletion time: %fs.\n", t);
					break; //RETHINK, but I think this s correct; and then new combin. of cross/mut is generated

				}
				//goto end; //known result

				int best_arr_size = best_arr.size();

				if (best_arr_size < 100000) continue; //continue to next generation; otherwise, check the no_changes clause below

				if (best_arr_size % 5000 == 0)
				{
					mut_prob += 0.1; // may COMMENT this OUT
					if (best_arr[94999] == best_arr[99999])
					{
						no_changes = true;
						break; // exits this for loop
					}

				}



				if (no_changes) // if the no_changes flag is on from loop above - then we exit the generations loop and move no next pair cross/mut
				{
					myfile2 << "Current best is: ";
					myfile2 << best;
					myfile2 << ",";
					myfile2 << "Current mut_prob is: ";
					myfile2 << mut_prob;
					myfile2 << "\n";


					clock_t end = clock();
					double t = (double)(end - start) / CLOCKS_PER_SEC;
					printf("\nCompletion time: %fs.\n", t);
					break;
				}


				//best = 0; //mb we can comment this

			} //this is the closing of generations loop


//			mutation_cycle++;


			// if solutions has been found write current mutaiton and cross_prob pair into structure
//		} // mutation iteration with fixed step ended

//		cross_cycle++;


//	} // crossover iteration with fixed step ended

	myfile1 << "\n";
	myfile1 << "end of" << file_path << "processing";

	myfile2 << "\n";
	myfile2 << "end of" << file_path << "processing";

	myfile1.close();
	myfile2.close();

//	} // this is end of one instance processing


//		end:
	printf("\n\n");





	return 0; // this is end of main()
}

