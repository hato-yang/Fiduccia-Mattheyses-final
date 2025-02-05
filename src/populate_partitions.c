#include "../include/main.h"

// Assumes no particular order for the CELL_array
// This seems to work well when the majority of cells are roughly the same, with large outliers
void segregate_cells_randomly(struct condensed *information)
{
	// Cells are divided randomly into two temporary partiton lists
	//	ex. If rand() >0.5, cell(i) goes to A, otherwise goes to B
	//  The cell areas are meanwhile added to the two totals
	//  At the end, the total areas are compared. If they don't match the desired ratio,
	//	lists are deleted and computer tries again.
	// If desired ratio met, the temporary lists are transcribed into the partition data_structures

	int i;

	struct dll *list_of_cells_A;
	struct dll *list_of_cells_B;
	/////////////////計算分布機率
	float x;
	x = (float)information->total_area / (information->total_area + information->total_area2);
	/////////////////
	/*int*/ long total_partition_areas[2];
	total_partition_areas[PARTITION_A] = 0;
	total_partition_areas[PARTITION_B] = 0;

	struct cell **CELL_array = information->CELL_array;

	// which partition the cell is assigned to PARTITION_A, PARTITION_B
	int partition_placement;
	// Loop until acceptable balance is found
	while (1)
	{
		list_of_cells_A = malloc(sizeof(struct dll));
		list_of_cells_B = malloc(sizeof(struct dll));

		// Seed random for different results (adding rand() prevents the seed from being reused in short time intervals)
		// Initialize the dlls
		initialize_dll(list_of_cells_A);
		initialize_dll(list_of_cells_B);

		srand(time(NULL) + rand());

		// Assign every cell to a partition
		for (i = 0; i < information->CELL_array_size; i++)
		{
			// Decide which partition the cell will go into
			// First option is random, repeat runs are based on FM_chromosome
			if (information->FM_chromosome != NULL)
			{
				partition_placement = information->FM_chromosome->gene_array[i];
			}
			else
			{
				float random_value = (float)rand() / RAND_MAX; // 生成 0 到 1 的隨機數
				partition_placement = (random_value < x);	   // 如果隨機數小於 x，則選擇 partition A
															   //    根據當前分區面積動態決定分配到哪個分區
															   // if (total_partition_areas[PARTITION_A] < total_partition_areas[PARTITION_B])
															   // {
															   // 	partition_placement = PARTITION_A;
															   // }
															   // else
															   // {
															   // 	partition_placement = PARTITION_B;
															   // }
			}
			// partition_placement = ((rand() % (int)(1.0 / (information->ratio))) == 0); // 原始隨機分配cell方法

			// total_partition_areas[partition_placement] += CELL_array[i]->area; // 原始代入面積寫法
			////////////////
			if (partition_placement == PARTITION_A)
			{
				total_partition_areas[partition_placement] += CELL_array[i]->area;
				// printf("cell# : %d cellarea : %d total.A : %ld\n", i, CELL_array[i]->area, total_partition_areas[partition_placement]);
			}
			if (partition_placement == PARTITION_B)
			{
				total_partition_areas[partition_placement] += CELL_array[i]->area2;
				// printf("cell# : %d cellarea : %d total.B : %ld\n", i, CELL_array[i]->area2, total_partition_areas[partition_placement]);
			}
			////////////////
			CELL_array[i]->partition = information->access_[partition_placement];
			CELL_array[i]->which_partition = partition_placement;

			if (partition_placement == PARTITION_A)
			{
				insert_node(list_of_cells_A, 0, CELL_array[i]);
			}
			else
			{
				insert_node(list_of_cells_B, 0, CELL_array[i]);
			}
		}
		/*int*/ long two_partition_area = 0;														  // 初始總面積計算
		two_partition_area = total_partition_areas[PARTITION_A] + total_partition_areas[PARTITION_B]; // 初始總面積計算
		printf("%ld   %ld   %ld   %ld\n", total_partition_areas[PARTITION_A], total_partition_areas[PARTITION_B], two_partition_area, two_partition_area / 2);
		// If the partition is within tolerance, break the loop and save to partition structs
		// Otherwise free dlls and try again.
		if (total_partition_areas[PARTITION_A] < ((two_partition_area / 2) + (information->tolerance_Macro * 3)) && total_partition_areas[PARTITION_A] > ((two_partition_area / 2) - (information->tolerance_Macro * 3)))
		// if (total_partition_areas[PARTITION_A] < 2838171970 && total_partition_areas[PARTITION_B] < 2838171970)//兩邊面積皆小於Die size
		{
			information->initial_area = total_partition_areas[PARTITION_A];
			information->initial_area2 = total_partition_areas[PARTITION_B];
			break;
		}
		// 原始代入面積寫法  information->desired_area 修改成 two_partition_area/2
		// Get ready for next loop
		garbage_collection_dll(list_of_cells_A, DO_NOT_DEALLOC_DATA);
		garbage_collection_dll(list_of_cells_B, DO_NOT_DEALLOC_DATA);
		total_partition_areas[PARTITION_A] = 0;
		total_partition_areas[PARTITION_B] = 0;
	}

	copy_cells_into_partitions(information->partition_A, information->partition_B, list_of_cells_A, list_of_cells_B, total_partition_areas[PARTITION_A], total_partition_areas[PARTITION_B]);

	garbage_collection_dll(list_of_cells_A, DO_NOT_DEALLOC_DATA);
	garbage_collection_dll(list_of_cells_B, DO_NOT_DEALLOC_DATA);
}

// Add cells, return dll of cutstate nets
void copy_cells_into_partitions(struct partition *partition_A, struct partition *partition_B, struct dll *list_of_cells_A, struct dll *list_of_cells_B, long total_partition_area_A, long total_partition_area_B)
{

	partition_A->total_partition_area = total_partition_area_A;

	// Go through each cell. Each cell has a list of nets. Go through each net. Add 1 to the number_cells_in_partition_X;
	struct node *temp_node;
	struct cell *temp_cell;

	// Access the first data node
	temp_node = ((struct node *)list_of_cells_A->head)->next;
	// Go through list of cells
	while (temp_node != list_of_cells_A->tail)
	{
		temp_cell = temp_node->data_structure;
		// Add cell to partition
		insert_node(partition_A->cells_in_partition, 0, temp_cell);
		update_net_partition_count(temp_cell, PARTITION_A);
		// Move to next cell
		temp_node = temp_node->next;
	}

	partition_B->total_partition_area = total_partition_area_B;

	// Access the first data node
	temp_node = ((struct node *)list_of_cells_B->head)->next;
	// Go through list of cells
	while (temp_node != list_of_cells_B->tail)
	{
		temp_cell = temp_node->data_structure;
		// Add cell to partition
		insert_node(partition_B->cells_in_partition, 0, temp_cell);
		update_net_partition_count(temp_cell, PARTITION_B);
		// Move to next cell
		temp_node = temp_node->next;
	}
}

// Go through each net and increment the appropriate counter variable
void update_net_partition_count(struct cell *assigned_cell, partition_type partition)
{
	struct dll *netlist = assigned_cell->nets;
	struct node *temp_net_node = ((struct node *)netlist->head)->next;
	struct net *temp_net;
	while (temp_net_node != netlist->tail)
	{
		temp_net = temp_net_node->data_structure;

		temp_net->num_cells_in_[partition] += 1;
		// Move to next net
		temp_net_node = temp_net_node->next;
	}
}

int calculate_initial_cutstate(struct net **NET_array, int NET_array_size, struct condensed *information)
{
	// Go through each net in NET_array, check to see if net has at least one cell in each

	struct net *temp_net;
	int cutstate_count = 0;
	int i;
	for (i = 0; i < NET_array_size; i++)
	{
		temp_net = NET_array[i];
		if ((temp_net->num_cells_in_[PARTITION_A] > 0) && (temp_net->num_cells_in_[PARTITION_B] > 0))
			cutstate_count++;
	}
	//	printf("Initial cutstate value: %d\n", cutstate_count);
	information->current_cutstate = cutstate_count;
	return cutstate_count;
}
