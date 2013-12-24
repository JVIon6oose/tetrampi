#include <stdio.h>
#include <vector>
#include <mpi.h>
#include <time.h>

int main(int argc, char *argv[])
{
	clock_t start, finish;
	double time;

	start = clock();

	MPI_Request request;
	MPI_Status status;
	char sig[1];
	char procname[MPI_MAX_PROCESSOR_NAME];
	bool state = true;
	unsigned long long int buf[1];
	unsigned long long int foo[1];
	int namelen, rank, numprocs;
	unsigned long long int i;

	//grain
	int grain = 1000;

	//grain for k=2
	int grain2 = 1000;

	//grain for k=3
	int grain3 = 1000;

	//testing limit
	unsigned long long int  max = 8000032000;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Get_processor_name(procname, &namelen);

	buf[0] = 8000000000;

	//MASTER process
	if(rank == 0)
	{
		int flag = 0;

		//send index to each

		for(i = 1; i < numprocs; i++)
		{
			MPI_Send(&buf, 1, MPI_UNSIGNED_LONG_LONG, i, 0, MPI_COMM_WORLD);
			buf[0] = buf[0] + grain;
		}
		//continually listen and hand out new grains if pending message

		while(state)
		{

			for(i = 1; i < numprocs; i++)
			{
				MPI_Iprobe(i, 0, MPI_COMM_WORLD, &flag, &status);


				if(flag == 1)
				{

					MPI_Recv(&foo, 1, MPI_UNSIGNED_LONG_LONG, i, 0, MPI_COMM_WORLD, &status);

					std::cout << "Processed " << foo[0] + grain << ".\n";

					MPI_Send(&buf, 1, MPI_UNSIGNED_LONG_LONG, i, 0, MPI_COMM_WORLD);
					buf[0] = buf[0] + grain;


					if(buf[0] >= max)
					{
						state = false;
					}
				}

			}

		}


		for(i = 1; i < numprocs; i++)
		{
			MPI_Probe(i, 0, MPI_COMM_WORLD, &status);
			MPI_Recv(&foo, 1, MPI_UNSIGNED_LONG_LONG, i, 0, MPI_COMM_WORLD, &status);

			std::cout << "Processed " << foo[0] + grain << "\n";

		}

		std::cout << procname << " MASTER has finished.\n";

		buf[0] = -1;

		//send flag to processers to call MPI_Finalize()
		for(i = 1; i < numprocs; i++)
		{
			MPI_Send(&buf, 1, MPI_UNSIGNED_LONG_LONG, i, 0, MPI_COMM_WORLD);
		}

		finish = clock();

		time = (double) finish - start / CLOCKS_PER_SEC;

		std::cout << "Time expired: " << time << "\n";

	}

	//slave process
	else
	{
		std::vector<unsigned long long int> tet_1;
		std::vector<unsigned long long int> tet_2;
		std::vector<unsigned long long int> tet_3;
		std::vector<unsigned long long int> temp_tet_3;
		unsigned long long int num = 0;
		unsigned long long int num2 = 0;
		unsigned long long int num3 = 0;
		unsigned long long int num4 = 0;
		unsigned long long int num5 = 0;
		bool found = false;
		int g2 = grain2;
		int g3 = grain3;
		int j;
		int k;
		int l;
		int m;
		int n;
		int o;
		int p;
		int t1 = 0;
		int t2 = 0;
		int t3 = 0;
		int tt3 = 0;

		//receive index
		MPI_Irecv(&buf, 1, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD, &request);
		MPI_Wait(&request, &status);

		i = 1;
		num = 1;

		/*******************************************************************************

		Create tet_1 sequence to max, simple while loop. Entire sequence lies in memory
		(as a vector).

		*******************************************************************************/

		while (num <= max)
		{
			tet_1.push_back(num);
			i++;
			num = i * (i + 1) * (i + 2);
			num = num/6;
		}

		tet_1.push_back(num);
		tet_2.push_back(0);
		tet_3.push_back(0);

		while(state)
		{

			/*******************************************************************************

			Create tet_2 via tet_1. Sort by insertion while generating ith sum. Discard
			doubles within sequence, which are found by sorting.

			*******************************************************************************/

			if((g2 == grain2) || (tet_2[tet_2.size() - 1] < buf[0] + grain))
			{

				i = 0;
				j = 0;
				k = 0;
				l = 0;
				m = 0;
				num = 0;
				t2 = 0;

				tet_2.clear();

				//find lower limit for grain2 (index of max combinatorial sum less than N)

				while(tet_1[i] * 2 <= buf[0])
				{
					i++;
				}

				l = i;

				//find upper limit for grain2 (index of min combinatorial sum less than N + grain)

				while((tet_1[l] + 1 <= buf[0] + grain2) && (l < tet_1.size()))
				{
					l++;
				}

				//first case tet_2 generation

				for(j = 0; j <= i; j++)
				{

					num = tet_1[i] + tet_1[j];

					if((num >= buf[0]) && (num <= max))
					{
						tet_2.push_back(num);
					}
				}

				i++;

				//first cases tet_2 generation, impose lower limit

				while(tet_1[i] + 1 < buf[0])
				{

					//k is current insertion point, m is ith insertion point

					j = 0;
					num = tet_1[i] + tet_1[j];

					while((k < tet_2.size()) && (tet_2[k] < num))
					{
						k++;
					}

					m = k;

					for(j = 0; j <= i; j++)
					{
						num = tet_1[i] + tet_1[j];

						//skip multiples, sort by insertion (not insertion sort)
						//k is current insertion point, n is ith insertion point

						if(num > tet_2[tet_2.size() - 1])
						{
							if((num != tet_2[tet_2.size() - 1]) && (num >= buf[0]) && (num <= buf[0] + grain2) && (num <= max))
							{
								tet_2.push_back(num);
							}
						}

						else
						{
							while((k < tet_2.size()) && (tet_2[k] < num))
							{
								k++;
							}

							if((tet_2[k] != num) && (num >= buf[0]) && (num <= buf[0] + grain2) && (num <= max))
							{
								tet_2.insert(tet_2.begin() + k, num);
							}
						}
					}

					k = m;
					i++;

				}

				//middle cases tet_2 generation

				while((tet_1[i] * 2 <= buf[0] + grain2) && (tet_1[i] * 2 <= max))
				{

					j = 0;
					num = tet_1[i] + tet_1[j];

					while((k < tet_2.size()) && (tet_2[k] < num))
					{
						k++;
					}

					m = k;

					for(j = 0; j <= i; j++)
					{
						num = tet_1[i] + tet_1[j];

						if(num > tet_2[tet_2.size() - 1])
						{
							if(num != tet_2[tet_2.size() - 1])
							{
								tet_2.push_back(num);
							}
						}

						else
						{
							while((k < tet_2.size()) && (tet_2[k] < num))
							{
								k++;
							}

							if(tet_2[k] != num)
							{
								tet_2.insert(tet_2.begin() + k, num);
							}
						}
					}

					k = m;
					i++;

				}

				//last cases tet_2 generation

				while(i < l)
				{

					j = 0;
					num = tet_1[i] + tet_1[j];

					while((k < tet_2.size())  && (tet_2[k] < num))
					{
						k++;
					}

					m = k;

					for(j = 0; j <= i; j++)
					{

						num = tet_1[i] + tet_1[j];

						if(num > tet_2[tet_2.size() - 1])
						{
							if((num != tet_2[tet_2.size() - 1]) && (num <= buf[0] + grain2) && (num <= max))
							{
								tet_2.push_back(num);
							}
						}

						else
						{
							while((k < tet_2.size()) && (tet_2[k] < num))
							{
								k++;
							}

							if((tet_2[k] != num) && (num <= buf[0] + grain2) && (num <= max))
							{
								tet_2.insert(tet_2.begin() + k, num);
							}
						}

					}

					k = m;
					i++;
				}

				g2 = 0;

			}


			else
			{
				g2 = g2 + grain2;
			}

			/*******************************************************************************

			Create tet_3 via tet_1. Sort by insertion while generating ith sum. Discard
			doubles within sequence, which are found by sorting.

			*******************************************************************************/

			if(g3 == grain3 || (tet_3[tet_3.size() - 1] < buf[0] + grain))
			{

				i = 0;
				j = 0;
				k = 0;
				l = 0;
				m = 0;
				n = 0;
				num = 0;
				num2 = 0;
				t3 = 0;

				tet_3.clear();

				//allow for a search space for K == 4
				if(buf[0] != 0)
				{
					num = buf[0] - 1;
				}

				//find lower limit for grain3 (index of max combinatorial sum less than N - 1)

				while(tet_1[i] * 3 <= num)
				{
					i++;
				}

				l = i;

				//find upper limit for grain3 (index of min combinatorial sum less than N + grain)

				while((tet_1[l] + 2 <= buf[0] + grain3) && (l < tet_1.size()))
				{
					l++;
				}

				//first cases tet_3 generation, impose lower limit

				while(tet_1[i] + 2 < buf[0])
				{

					j = 0;
					n = 0;
					num = tet_1[i] + tet_1[j];
					num2 = num + tet_1[n];

					while((k < tet_3.size()) && (tet_3[k] < num2))
					{
						k++;
					}

					m = k;

					for(j = 0; j <= i; j++)
					{
						num = tet_1[i] + tet_1[j];

						for(n = j; n <= i; n++)
						{
							num2 = tet_1[n] + num;

							if(num2 > tet_3[tet_3.size() - 1])
							{
								if((num2 != tet_3[tet_3.size() - 1]) && (num2 >= buf[0]) && (num2 <= buf[0] + grain3) && (num2 <= max))
								{
									tet_3.push_back(num2);
								}
							}

							else
							{
								while((k < tet_3.size()) && (tet_3[k] < num2))
								{
									k++;
								}

								if((tet_3[k] != num2) && (num2 >= buf[0]) && (num2 <= buf[0] + grain3) && (num2 <= max))
								{
									tet_3.insert(tet_3.begin() + k, num2);
								}
							}

							k = m;
						}

					}

					k = m;
					i++;

				}

				//middle cases tet_3 generation

				while((tet_1[i] * 3 <= buf[0] + grain3) && (tet_1[i] * 3 <= max))
				{

					j = 0;
					n = 0;
					num = tet_1[i] + tet_1[j];
					num2 = num + tet_1[n];

					while((k < tet_3.size()) && (tet_3[k] < num2))
					{
						k++;
					}

					m = k;

					for(j = 0; j <= i; j++)
					{
						num = tet_1[i] + tet_1[j];

						for(n = j; n <= i; n++)
						{
							num2 = tet_1[n] + num;

							if(num2 > tet_3[tet_3.size() - 1])
							{
								if(num2 != tet_3[tet_3.size() - 1])
								{
									tet_3.push_back(num2);
								}
							}

							else
							{
								while((k < tet_3.size()) && (tet_3[k] < num2))
								{
									k++;
								}

								if(tet_3[k] != num2)
								{
									tet_3.insert(tet_3.begin() + k, num2);
								}
							}

							k = m;
						}

					}

					k = m;
					i++;

				}

				//last cases tet_3 generation

				while(i < l)
				{
					j = 0;
					n = 0;
					num = tet_1[i] + tet_1[j];
					num2 = num + tet_1[n];

					while((k < tet_3.size())  && (tet_3[k] < num2))
					{
						k++;
					}

					m = k;

					for(j = 0; j <= i; j++)
					{
						num = tet_1[i] + tet_1[j];

						for(n = j; n <= i; n++)
						{
							num2 = tet_1[n] + num;

							if(num2 > tet_3[tet_3.size() - 1])
							{
								if((num2 != tet_3[tet_3.size() - 1]) && (num2 <= buf[0] + grain3) && (num2 <= max))
								{
									tet_3.push_back(num2);
								}
							}

							else
							{
								while((k < tet_3.size()) && (tet_3[k] < num2))
								{
									k++;
								}

								if((tet_3[k] != num2) && (num2 <= buf[0] + grain3) && (num2 <= max))
								{
									tet_3.insert(tet_3.begin() + k, num2);
								}
							}

							k = m;


						}

					}

					k = m;
					i++;

				}

				g3 = 0;
			}

			else
			{
				g3 = g3 + grain3;
			}



			/******************************************************************************

			Binary search arrays, tet_1, tet_2, tet_3.

			******************************************************************************/


			if(buf[0] + grain > max)
			{
				num = max + 1;
			}

			else
			{
				num = buf[0] + grain;
			}

			if(buf[0] == 0)
			{
				buf[0] = 1;
			}

			for(i = buf[0]; i < num; i++)
			{

				k = tet_1.size() - 1;
				j = t1;

				//binary search tet_1

				while(k >= j)
				{
					l = (k + j)/2;

					if(tet_1[l] < i)
					{
						j = l + 1;
					}

					else if(tet_1[l] > i)
					{
						k = l - 1;
					}

					else
					{
						k = j - 1;
						t1 = j;
						found = true;
					}
				}

				//binary search tet_2

				if(!found)
				{
					k = tet_2.size() - 1;
					j = t2;

					while(k >= j)
					{
						l = (k + j)/2;

						if(tet_2[l] < i)
						{
							j = l + 1;
						}

						else if(tet_2[l] > i)
						{
							k = l - 1;
						}

						else
						{
							k = j - 1;
							t2 = j;
							found = true;
						}
					}
				}

				//binary search tet_3

				if(!found)
				{
					k = tet_3.size() - 1;
					j = t3;

					while(k >= j)
					{
						l = (k + j)/2;

						if(tet_3[l] < i)
						{
							j = l + 1;
						}

						else if(tet_3[l] > i)
						{
							k = l - 1;
						}

						else
						{
							k = j - 1;
							t3 = j;
							found = true;
						}
					}
				}

				/******************************************************************************

				Search heuristics for tet_4 numbers.

				******************************************************************************/

				//if N - 1 is in tet_3, N is a tet_4 number.

				if(!found)
				{
					k = tet_3.size() - 1;
					j = 0;

					while(k >= j)
					{
						l = (k + j)/2;

						if(tet_3[l] < i - 1)
						{
							j = l + 1;
						}

						else if(tet_3[l] > i - 1)
						{
							k = l - 1;
						}

						else
						{
							k = j - 1;
							found = true;
						}
					}
				}

				//iteratively search for N - tet_1[i] in tet_3, create temp_tet_3 when necessary

				if(!found)
				{
					temp_tet_3.push_back(0);
					j = tet_1.size() - 1;
					num3 = 0;

					//find greatest tet_1 less than N

					while(tet_1[j] > i)
					{
						j--;
					}

					while(j >= 1 && !found)
					{

						num2 = i - tet_1[j];		

						//binary search tet_3 if it may include N

						if(num2 >= buf[0])
						{
							k = tet_3.size() - 1;
							m = 0;

							while(k >= m)
							{
								l = (k + m)/2;

								if(tet_3[l] < num2)
								{
									m = l + 1;
								}

								else if(tet_3[l] > num2)
								{
									k = l - 1;
								}

								else
								{
									k = m - 1;
									found = true;
								}
							}
						}

						//create temp_tet_3 otherwise so that it may include N

						else
						{

							//if current temp_tet_3, do binary search

							if((num2 >= num3) && (num3 != 0) && (num2 <= num3 + grain3))
							{
								k = temp_tet_3.size() - 1;
								m = t3;

								while(k >= m)
								{
									l = (k + m)/2;

									if(temp_tet_3[l] < num2)
									{
										m = l + 1;
									}

									else if(temp_tet_3[l] > num2)
									{
										k = l - 1;
									}

									else
									{
										k = m - 1;
										tt3 = m;
										found = true;
									}
								}

							}

							//else create temp_tet_3

							else
							{

								temp_tet_3.clear();

								num3 = 0;
								num4 = 0;
								num5 = 0;
								k = 0;
								l = 0;
								m = 0;
								n = 0;
								o = 0;
								p = 0;
								tt3 = 0;

								//i is N, num2 is N - K1,j is iterator for N - K1
								//num is iterator limit for searching N grain
								//changed i, num and num 2 to o, num3 and num4.

								if(grain3 > num2)
								{
									num3 = 1;
								}

								else
								{
									num3 = num2;
									num3 = num3 - grain3;
								}

								//find lower limit for grain3 (index of max combinatorial sum less than N - 1)

								while(tet_1[o] * 3 <= num3)
								{
									o++;
								}

								l = o;

								//find upper limit for grain3 (index of min combinatorial sum less than N + grain)

								while((tet_1[l] + 2 <= num3 + grain3) && (l < tet_1.size()))
								{
									l++;
								}

								//first cases temp_tet_3 generation, impose lower limit

								while(tet_1[o] + 2 < num3)
								{

									p = 0;
									n = 0;
									num4 = tet_1[o] + tet_1[p];
									num5 = num4 + tet_1[n];

									while((k < temp_tet_3.size()) && (temp_tet_3[k] < num5))
									{
										k++;
									}

									m = k;

									for(p = 0; p <= o; p++)
									{
										num4 = tet_1[o] + tet_1[p];

										for(n = p; n <= o; n++)
										{
											num5 = tet_1[n] + num4;

											if(num5 > temp_tet_3[temp_tet_3.size() - 1])
											{
												if((num5 != temp_tet_3[temp_tet_3.size() - 1]) && (num5 >= num3) && (num5 <= num3 + grain3) && (num5 <= max))
												{
													temp_tet_3.push_back(num5);
												}
											}

											else
											{
												while((k < temp_tet_3.size()) && (temp_tet_3[k] < num5))
												{
													k++;
												}

												if((temp_tet_3[k] != num5) && (num5 >= num3) && (num5 <= num3 + grain3) && (num5 <= max))
												{
													temp_tet_3.insert(temp_tet_3.begin() + k, num5);
												}
											}

											k = m;
										}

									}

									k = m;
									o++;
								}

								//middle cases temp_tet_3 generation

								while((tet_1[o] * 3 <= num3 + grain3) && (tet_1[o] * 3 <= max))
								{

									p = 0;
									n = 0;
									num4 = tet_1[o] + tet_1[p];
									num5 = num4 + tet_1[n];

									while((k < temp_tet_3.size()) && (temp_tet_3[k] < num5))
									{
										k++;
									}

									m = k;

									for(p = 0; p <= o; p++)
									{
										num4 = tet_1[o] + tet_1[p];

										for(n = p; n <= o; n++)
										{
											num5 = tet_1[n] + num4;

											if(num5 > temp_tet_3[temp_tet_3.size() - 1])
											{
												if(num5 != temp_tet_3[temp_tet_3.size() - 1])
												{
													temp_tet_3.push_back(num5);
												}
											}

											else
											{
												while((k < temp_tet_3.size()) && (temp_tet_3[k] < num5))
												{
													k++;
												}

												if(temp_tet_3[k] != num5)
												{
													temp_tet_3.insert(temp_tet_3.begin() + k, num5);
												}
											}

											k = m;
										}

									}

									k = m;
									o++;


								}

								//last cases temp_tet_3 generation

								while(o < l)
								{

									p = 0;
									n = 0;
									num4 = tet_1[o] + tet_1[p];
									num5 = num4 + tet_1[n];

									while((k < temp_tet_3.size()) && (temp_tet_3[k] < num5))
									{
										k++;
									}

									m = k;

									for(p = 0; p <= o; p++)
									{
										num4 = tet_1[o] + tet_1[p];

										for(n = p; n <= o; n++)
										{
											num5 = tet_1[n] + num4;

											if(num5 > temp_tet_3[temp_tet_3.size() - 1])
											{
												if((num5 != temp_tet_3[temp_tet_3.size() - 1]) && (num5 <= num3 + grain3) && (num5 <= max))
												{
													temp_tet_3.push_back(num5);
												}
											}

											else
											{
												while((k < temp_tet_3.size()) && (temp_tet_3[k] < num5))
												{
													k++;
												}

												if((temp_tet_3[k] != num5) && (num5 <= num3 + grain3) && (num5 <= max))
												{
													temp_tet_3.insert(temp_tet_3.begin() + k, num5);
												}
											}

											k = m;
										}

									}

									k = m;
									o++;

								}

								k = temp_tet_3.size() - 1;
								m = 0;

								while(k >= m)
								{
									l = (k + m)/2;
									m = tt3;

									if(temp_tet_3[l] < num2)
									{
										m = l + 1;
									}

									else if(temp_tet_3[l] > num2)
									{
										k = l - 1;
									}

									else
									{
										k = m - 1;
										tt3 = m;
										found = true;
									}
								}
							}

						}

						j--;
					}
				}

				if(!found)
				{
					//This is for k >= 5.
					std::cout << procname << " did not find " << i << "\n";

				}

				temp_tet_3.clear();

				found = false;

			}

			found = false;

			foo[0] = buf[0];

			MPI_Send(&foo, 1, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD);
			MPI_Recv(&buf, 1, MPI_UNSIGNED_LONG_LONG, 0, 0, MPI_COMM_WORLD, &status);

			if(buf[0] == -1)
			{
				state = false;
			}
		}
	}

	MPI_Finalize();
}
