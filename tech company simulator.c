#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
int tutorialpart = 0;
char company_name[50];
char cpu_name[11];
int new_user = 1;
double money = 1000.0;
int int_answer;
int cores;
int cost = 0;
int threads;
double speed;
double o_clock_speed;
int total_products;
int total_employees;
int input;
char charinput[50];
double overheat;

typedef struct{
	char name[11];
	int cores;
	int threads;
	double base_speed;
	double overclock_speed;
	double overheat;
	int cost;
	} CPU;
	
#define MAX_CPUS 100

CPU cpus[MAX_CPUS];
int cpu_count = 0;

void company();
void main_menu();
void list_cpus()
{
    if (cpu_count == 0)
    {
        printf("You have not designed any CPUs yet.\n");
        return;
    }

    for (int i = 0; i < cpu_count; i++)
    {
        printf("\nCPU #%d\n", i + 1);
        printf("Name: %s\n", cpus[i].name);
        printf("Cores: %d | Threads: %d\n",
               cpus[i].cores, cpus[i].threads);
        printf("Base clock: %.2f GHz\n", cpus[i].base_speed);
        printf("Overclock: %.2f GHz\n", cpus[i].overclock_speed);
        printf("Overheat: %.2f%%\n", cpus[i].overheat);
        printf("Cost: $%d\n", cpus[i].cost);
        printf("===================\n");
      
    }
    printf("press 1 to return to company stats and 2 to return main menu.");
    scanf("%d", &input);
    switch(input){
		case 1: 
		    printf("\033[2J\033[H");
	fflush(stdout);
			company();
			break;
		case 2: 
		    printf("\033[2J\033[H");
	fflush(stdout);
			main_menu();
			break;
		default:
			printf("invalid input");
			break;
		}
}

void build_cpu(){
	cost = 0;
    printf("You have %.2lf dollars\n", money);


    do {
        printf("How many cores should your CPU have: 2 ($100/unit), 4 ($250/unit), 8 ($500/unit): ");
        
        if (scanf("%d", &int_answer) != 1) {
            int_answer = 0;
        }
        while (getchar() != '\n');

        if (int_answer == 2) {
            cores = 2;
            cost += 100;
            break;
        } else if (int_answer == 4) {
            cores = 4;
            cost
             += 250;
            break;
        } else if (int_answer == 8) {
            cores = 8;
            cost += 500;
            break;
        } else {
            printf("You did not select a valid number of cores [remember: answer 2, 4, or 8].\n");
        }
    } while (1);
    
    threads = cores * 2;
    if (threads == 4) {
        cost += 50;
    } else if (threads == 8) {
        cost += 150;
    } else if (threads == 16) {
        cost += 300;
    } else {
        printf("Error calculating threads. Current rendition of threads is %d\n", threads);
    }
    printf("Selected: %d cores | %d threads | Cost: $%d\n", cores, threads, cost);

    printf("Select over-clock speed. Be warned: if you set it too high your CPUs will gain a tendency to fry themselves and you'll lose profit and buyers.\n");

    speed = 2.8 + (cores * 0.25);

    printf("Your base speed is %.2f GHz. What will you set the max overclock to?\n", speed);
    printf("Enter overclock speed (GHz): ");
    scanf("%lf", &o_clock_speed);
     

		double increase = (o_clock_speed - speed) / speed;

			if (increase < 0)
		{
			increase = 0;
							}

	overheat = increase * increase * 250;
    while (getchar() != '\n');

    printf("Enter CPU name: ");
    fgets(cpu_name, sizeof(cpu_name), stdin);
    if (strchr(cpu_name, '\n') == NULL) {
        while (getchar() != '\n');
    } else {
        cpu_name[strcspn(cpu_name, "\n")] = '\0';
    }

    printf("\n%s has %d cores, %d threads, and a %.2lf GHz base speed with a possibility of %.2lf GHz overclocking, costing $%d. COMING TO MARKET in 3!\n",
           cpu_name, cores, threads, speed, o_clock_speed, cost);
           if (cpu_count < MAX_CPUS)
{
    strcpy(cpus[cpu_count].name, cpu_name);
    cpus[cpu_count].cores = cores;
    cpus[cpu_count].threads = threads;
    cpus[cpu_count].base_speed = speed;
    cpus[cpu_count].overclock_speed = o_clock_speed;
    cpus[cpu_count].overheat = overheat;
    cpus[cpu_count].cost = cost;

    cpu_count++;
}
else
{
    printf("You cannot store any more CPU designs!\n");
}
           total_products ++;
    sleep(3);
    
    printf("\033[2J\033[H");
	fflush(stdout);
	}

void main_menu();
void produce(){
	printf("money: %lf\n", money);
	printf("What do you want to create[cpu/gpu]? ");
	scanf("%49s", charinput);
	if (strcmp(charinput, "cpu") == 0){
		build_cpu();
		}else if (strcmp(charinput, "gpu") == 0){}
		
	}

void main_menu(){
	int running = 1;
	
	while (running){
	printf("%s\n", company_name);
	printf("Company:\n");
	printf("money: %.2lf\n", money);
	printf("products: %d\n", total_products);
	printf("Employees: %d\n", total_employees);
	printf("----------------------\n");
	printf("commands\n");
	printf("[1]Develop\n");
	printf("[2]Produce\n");
	printf("[3]Sell\n");
	printf("[4]Research\n");
	printf("[5]Company\n");
	printf("-----------------------\n");
	printf("Input: ");
	scanf("%d", &input);
	switch(input){
		case 1: 
		//Develop();
		break;
		
		case 2:
			produce();
			break;
			
		case 3:
			//sell();
			break;
			
		case 4:
			//research();
			break;
			
		case 5:
		      printf("\033[2J\033[H");
	fflush(stdout); 
			company();
			break;
		default:
			printf("not a valid input\n");
			break;
		}
	}
}

void launcher(){
	
	printf("===========================\n");
	printf("  tech company simulator    \n");
	printf("===========================\n");
	printf("[1]lauch simulator\n");
	printf("[2]help/documentation\n");
	printf("[3]Exit\n");
	printf("===========================\n");
	printf("input: ");
	scanf("%d", &input);
	
	switch(input){
		case 1:
    printf("Enter your company name: ");

    while (getchar() != '\n');

    fgets(company_name, sizeof(company_name), stdin);
    company_name[strcspn(company_name, "\n")] = '\0';

    main_menu();
    break;
			
		case 2:
			//help/documentation_page();
			break;
			
		case 3:
			_exit(1);
	}
	}void company(){
	printf("=======================\n");
	printf("%s stats/products\n", company_name);
	printf("=======================\n");
	printf("[1] veiw products\n");
	printf("[2] veiw profit\n");
	printf("[3] veiw reports/issues\n");
	printf("=======================\n");
	printf("input: ");
	scanf("%d", &input);
	
	switch(input){
		case 1:
		list_cpus();
		break;
		
		case 2: 
			
		}
	}
	
int main() {
    if (new_user == 1) {
        launcher();
    }

    return 0;
}
