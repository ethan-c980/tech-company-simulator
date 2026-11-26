#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

double marketprice;
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
double profitm = 0.5;

void erase_screen(void) {
    printf("\033[2J\033[H");
    fflush(stdout);
}

typedef struct {
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



typedef struct {
    char name[16];
    int architecture;
    double architecture_efficiency;
    int compute_units;
    double core_clock;       // MHz
    double max_core_clock;  // MHz
    int vram;               // GB
    double cooling;          // cooling rating, percent
    int cost;
    int tdp;                // watts
} GPU;


#define MAX_GPUS 100
GPU gpus[MAX_GPUS];
int gpu_count = 0;



void company(void);
void main_menu(void);
void launcher(void);
void help_documentation(void);
void build_cpu(void);
void build_gpu(void);
void tutorial(void);
void list_cpus(void);
void list_gpus(void);
void produce(void);

/* Consume the rest of a line after scanf, safely even at EOF. */
void clear_input_line(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}


void list_cpus(void) {
    if (cpu_count == 0) {
        printf("You have not designed any CPUs yet.\n");
    } else {
        for (int i = 0; i < cpu_count; i++) {
            printf("\nCPU #%d\n", i + 1);
            printf("Name: %s\n", cpus[i].name);
            printf("Cores: %d | Threads: %d\n", cpus[i].cores, cpus[i].threads);
            printf("Base clock: %.2f GHz\n", cpus[i].base_speed);
            printf("Overclock: %.2f GHz\n", cpus[i].overclock_speed);
            printf("Overheat: %.2f%%\n", cpus[i].overheat);
            printf("Cost: $%d\n", cpus[i].cost);
            printf("===================\n");
        }
    }

    printf("Press 1 to return to company and 2 to return main menu: ");
    if (scanf("%d", &input) != 1) {
        clear_input_line();
        printf("Invalid input. Returning to company menu.\n");
        company();
        return;
    }
    clear_input_line();
    switch (input) {
        case 1:
            erase_screen();
            company();
            break;
        case 2:
            erase_screen();
            main_menu();
            break;
        default:
            printf("Invalid input. Returning to company menu.\n");
            company();
            break;
    }
}


void list_gpus(void) {
    if (gpu_count == 0) {
        printf("You have not designed any GPUs yet.\n");
    } else {
        for (int i = 0; i < gpu_count; i++) {
            printf("\nGPU #%d\n", i + 1);
            printf("Name: %s\n", gpus[i].name);
            printf("Architecture tier: %d (efficiency %.2f)\n",
                   gpus[i].architecture, gpus[i].architecture_efficiency);
            printf("Compute units: %d | VRAM: %d GB\n",
                   gpus[i].compute_units, gpus[i].vram);
            printf("Core clock: %.2f MHz\n", gpus[i].core_clock);
            printf("Max clock: %.2f MHz\n", gpus[i].max_core_clock);
            printf("Cooling rating: %.2f%%\n", gpus[i].cooling);
            printf("TDP: %d W\n", gpus[i].tdp);
            printf("Cost: $%d\n", gpus[i].cost);
            printf("===================\n");
        }
    }

    printf("Press 1 to return to company stats and 2 to return main menu: ");
    if (scanf("%d", &input) != 1) {
        clear_input_line();
        printf("Invalid input. Returning to company menu.\n");
        company();
        return;
    }
    clear_input_line();
    switch (input) {
        case 1:
            erase_screen();
            company();
            break;
        case 2:
            erase_screen();
            main_menu();
            break;
        default:
            printf("Invalid input. Returning to company menu.\n");
            company();
            break;
    }
}


void build_cpu(void) {
    cost = 0;
    printf("You have %.2lf dollars\n", money);

    do {
        printf("How many cores should your CPU have: 2 ($100), 4 ($250), 8 ($500): ");
        if (scanf("%d", &int_answer) != 1) {
            int_answer = 0;
        }
        clear_input_line();

        if (int_answer == 2) {
            cores = 2;
            cost += 100;
            break;
        } else if (int_answer == 4) {
            cores = 4;
            cost += 250;
            break;
        } else if (int_answer == 8) {
            cores = 8;
            cost += 500;
            break;
        } else {
            printf("You did not select a valid number of cores (enter 2, 4, or 8).\n");
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
        printf("Error calculating threads. Current thread count is %d\n", threads);
    }
    printf("Selected: %d cores | %d threads | Cost: $%d\n", cores, threads, cost);

    printf("Select overclock speed. Be warned: if you set it too high your CPUs will gain a tendency to overheat and you'll lose profit and buyers.\n");
    speed = 2.8 + (cores * 0.25);
    printf("Your base speed is %.2f GHz. What will you set the max overclock to?\n", speed);

    do {
        printf("Enter overclock speed (GHz): ");
        if (scanf("%lf", &o_clock_speed) != 1) {
            o_clock_speed = -1;
        }
        clear_input_line();
        if (o_clock_speed < speed) {
            printf("Overclock must be at least the base speed (%.2f GHz).\n", speed);
        }
    } while (o_clock_speed < speed);

    double increase = (o_clock_speed - speed) / speed;
    if (increase < 0) increase = 0;
    overheat = increase * increase * 250;

    printf("Enter CPU name (max 10 characters): ");
    if (!fgets(cpu_name, sizeof(cpu_name), stdin)) {
        cpu_name[0] = '\0';
    }
    cpu_name[strcspn(cpu_name, "\n")] = '\0';

    printf("\n%s has %d cores, %d threads, %.2f GHz base speed and %.2f GHz max overclock, costing $%d. COMING TO MARKET in 3!\n",
           cpu_name, cores, threads, speed, o_clock_speed, cost);

    if (cpu_count < MAX_CPUS) {
        strncpy(cpus[cpu_count].name, cpu_name, sizeof(cpus[cpu_count].name) - 1);
        cpus[cpu_count].name[sizeof(cpus[cpu_count].name) - 1] = '\0';
        cpus[cpu_count].cores = cores;
        cpus[cpu_count].threads = threads;
        cpus[cpu_count].base_speed = speed;
        cpus[cpu_count].overclock_speed = o_clock_speed;
        cpus[cpu_count].overheat = overheat;
        cpus[cpu_count].cost = cost;
        cpu_count++;
        total_products++;
    } else {
        printf("You cannot store any more CPU designs!\n");
    }

    sleep(3);
    erase_screen();
}


void build_gpu(void) {
    if (gpu_count >= MAX_GPUS) {
        printf("You cannot store any more GPU designs!\n");
        return;
    }

    GPU new_gpu = {0};
    int architecture_choice;
    int vram_choice;
    int units_choice;
    double clock_choice;

    printf("\n--- GPU Design ---\n");
    printf("You have %.2f dollars\n", money);
    printf("Choose architecture tier:\n");
    printf("1) Basic       (efficiency 0.80, base cost $200)\n");
    printf("2) Balanced    (efficiency 1.00, base cost $400)\n");
    printf("3) Advanced    (efficiency 1.20, base cost $700)\n");
    printf("4) Enthusiast (efficiency 1.40, base cost $1000)\n");
    printf("Architecture choice: ");
    if (scanf("%d", &architecture_choice) != 1) {
        clear_input_line();
        printf("Invalid architecture selection. GPU design cancelled.\n");
        return;
    }
    clear_input_line();

    switch (architecture_choice) {
        case 1: new_gpu.architecture = 1; new_gpu.architecture_efficiency = 0.80; cost = 200; break;
        case 2: new_gpu.architecture = 2; new_gpu.architecture_efficiency = 1.00; cost = 400; break;
        case 3: new_gpu.architecture = 3; new_gpu.architecture_efficiency = 1.20; cost = 700; break;
        case 4: new_gpu.architecture = 4; new_gpu.architecture_efficiency = 1.40; cost = 1000; break;
        default: printf("Invalid architecture selection. GPU design cancelled.\n"); return;
    }

    printf("Choose compute units (8, 16, 32, or 64): ");
    if (scanf("%d", &units_choice) != 1) {
        clear_input_line();
        printf("Invalid compute unit input. GPU design cancelled.\n");
        return;
    }
    clear_input_line();
    if (units_choice != 8 && units_choice != 16 && units_choice != 32 && units_choice != 64) {
        printf("Invalid compute unit selection. GPU design cancelled.\n");
        return;
    }
    new_gpu.compute_units = units_choice;
    cost += units_choice * 20;

    printf("Choose VRAM (4, 8, 12, or 16 GB): ");
    if (scanf("%d", &vram_choice) != 1) {
        clear_input_line();
        printf("Invalid VRAM input. GPU design cancelled.\n");
        return;
    }
    clear_input_line();
    if (vram_choice != 4 && vram_choice != 8 && vram_choice != 12 && vram_choice != 16) {
        printf("Invalid VRAM selection. GPU design cancelled.\n");
        return;
    }
    new_gpu.vram = vram_choice;
    cost += vram_choice * 25;

    printf("Choose base core clock (MHz, 900-2200): ");
    if (scanf("%lf", &clock_choice) != 1) {
        clear_input_line();
        printf("Invalid clock input. GPU design cancelled.\n");
        return;
    }
    clear_input_line();
    if (clock_choice < 900 || clock_choice > 2200) {
        printf("Clock is outside the allowed range. GPU design cancelled.\n");
        return;
    }
    new_gpu.core_clock = clock_choice;
    new_gpu.max_core_clock = clock_choice * 1.15;
    cost += (int)(clock_choice / 10.0);

    new_gpu.cooling = 60.0 + (new_gpu.architecture_efficiency * 10.0);
    new_gpu.tdp = 100 + (new_gpu.compute_units * 8) + (new_gpu.vram * 5);
    new_gpu.cost = cost;

    printf("Enter GPU name (max 15 characters): ");
    if (!fgets(new_gpu.name, sizeof(new_gpu.name), stdin)) {
        new_gpu.name[0] = '\0';
    }
    new_gpu.name[strcspn(new_gpu.name, "\n")] = '\0';

    printf("\nGPU design summary\n");
    printf("Name: %s\n", new_gpu.name);
    printf("Architecture tier: %d | Efficiency: %.2f\n", new_gpu.architecture, new_gpu.architecture_efficiency);
    printf("Compute units: %d | VRAM: %d GB\n", new_gpu.compute_units, new_gpu.vram);
    printf("Core clock: %.2f MHz | Max clock: %.2f MHz\n", new_gpu.core_clock, new_gpu.max_core_clock);
    printf("Cooling rating: %.2f%% | TDP: %d W\n", new_gpu.cooling, new_gpu.tdp);
    printf("Estimated design cost: $%d\n", new_gpu.cost);
    printf("COMING TO MARKET in 3!\n");

    gpus[gpu_count] = new_gpu;
    gpu_count++;
    total_products++;
    sleep(3);
    erase_screen();
}


void produce(void) {
    printf("Money: %.2f\n", money);
    printf("What do you want to create [cpu/gpu]? ");
    if (scanf("%49s", charinput) != 1) {
        clear_input_line();
        printf("Could not read selection.\n");
        return;
    }
    clear_input_line();
    if (strcmp(charinput, "cpu") == 0) {
        build_cpu();
    } else if (strcmp(charinput, "gpu") == 0) {
        build_gpu();
    } else {
        printf("Please type cpu or gpu.\n");
    }
}


void main_menu(void) {
    int running = 1;
    while (running) {
        printf("%s\n", company_name);
        printf("======================\n");
        printf("Company:\n");
        printf("Money: %.2lf\n", money);
        printf("Products: %d\n", total_products);
        printf("Employees: %d\n", total_employees);
        printf("----------------------\n");
        printf("Commands\n");
        printf("[1] Develop\n");
        printf("[2] Produce\n");
        printf("[3] Sell\n");
        printf("[4] Research\n");
        printf("[5] Company\n");
        printf("[6] Exit to launcher\n");
        printf("-----------------------\n");
        printf("Input: ");
        if (scanf("%d", &input) != 1) {
            clear_input_line();
            printf("Not a valid input.\n");
            continue;
        }
        clear_input_line();
        switch (input) {
            case 1:
                // Develop() will be added later.
                printf("Development menu is not implemented yet.\n");
                break;
            case 2:
                produce();
                break;
            case 3:
                Sell();
                break;
            case 4:
                // research() will be added later.
                printf("Research is not implemented yet.\n");
                break;
            case 5:
                erase_screen();
                company();
                break;
            case 6:
                running = 0;
                break;
            default:
                printf("Not a valid input.\n");
                break;
        }
    }
}


void tutorial(void) {
    tutorialpart = 0;
    cost = 0;
    tutorialpart++;
    printf("\nTutorial [%d/5]\n", tutorialpart);
    printf("Time to make your first part! First we will make a CPU (central processing unit).\n");
    printf("You have %.2lf dollars\n", money);

    do {
        printf("How many cores should your CPU have: 2 ($100), 4 ($250), 8 ($500): ");
        if (scanf("%d", &int_answer) != 1) int_answer = 0;
        clear_input_line();
        if (int_answer == 2) {
            cores = 2;
            cost += 100;
            break;
        } else if (int_answer == 4) {
            cores = 4;
            cost += 250;
            break;
        } else if (int_answer == 8) {
            cores = 8;
            cost += 500;
            break;
        } else {
            printf("You did not select a valid number of cores (enter 2, 4, or 8).\n");
        }
    } while (1);

    printf("Selected: %d cores | Cost: $%d\n", cores, cost);
    tutorialpart++;
    printf("\nTutorial [%d/5]\n", tutorialpart);
    printf("Based on your CPU core count, you will have double the threads. 2 cores = 4 threads (+$50), 4 cores = 8 threads (+$150), and 8 cores = 16 threads (+$300).\n");

    threads = cores * 2;
    if (threads == 4) cost += 50;
    else if (threads == 8) cost += 150;
    else if (threads == 16) cost += 300;
    else printf("Error calculating threads. Current thread count is %d\n", threads);
    printf("Selected: %d cores | %d threads | Cost: $%d\n", cores, threads, cost);

    tutorialpart++;
    printf("\nTutorial [%d/5]\n", tutorialpart);
    printf("Now decide your overclock speed. Setting it too high increases overheating risk in the game.\n");
    speed = 2.8 + (cores * 0.25);
    printf("Your base speed is %.2f GHz.\n", speed);
    do {
        printf("Enter overclock speed (GHz, at least %.2f): ", speed);
        if (scanf("%lf", &o_clock_speed) != 1) o_clock_speed = -1;
        clear_input_line();
        if (o_clock_speed < speed) printf("Overclock must be at least the base speed.\n");
    } while (o_clock_speed < speed);
    double increase = (o_clock_speed - speed) / speed;
    if (increase < 0) increase = 0;
    overheat = increase * increase * 250;

    tutorialpart++;
    printf("\nTutorial [%d/5]\n", tutorialpart);
    printf("Now name your CPU (max 10 characters).\n");
    printf("Enter CPU name: ");
    if (!fgets(cpu_name, sizeof(cpu_name), stdin)) cpu_name[0] = '\0';
    cpu_name[strcspn(cpu_name, "\n")] = '\0';

    printf("\n%s has %d cores, %d threads, %.2f GHz base speed and %.2f GHz max overclock, costing $%d. COMING TO MARKET in 3!\n",
           cpu_name, cores, threads, speed, o_clock_speed, cost);
    if (cpu_count < MAX_CPUS) {
        strncpy(cpus[cpu_count].name, cpu_name, sizeof(cpus[cpu_count].name) - 1);
        cpus[cpu_count].name[sizeof(cpus[cpu_count].name) - 1] = '\0';
        cpus[cpu_count].cores = cores;
        cpus[cpu_count].threads = threads;
        cpus[cpu_count].base_speed = speed;
        cpus[cpu_count].overclock_speed = o_clock_speed;
        cpus[cpu_count].overheat = overheat;
        cpus[cpu_count].cost = cost;
        cpu_count++;
        total_products++;
    } else {
        printf("You cannot store any more CPU designs!\n");
    }
    tutorialpart++;
    printf("\nTutorial [%d/5]\nTutorial CPU creation complete!\n", tutorialpart);
    sleep(3);
    erase_screen();
    help_documentation();
}


void help_documentation(void) {
    printf("==================\n");
    printf("Help/documentation\n");
    printf("==================\n");
    printf("[1] Tutorial\n");
    printf("[2] Research tree\n");
    printf("[3] CPU/GPU info\n");
    printf("[4] Return to launcher\n");
    printf("==================\n");
    printf("Input: ");
    if (scanf("%d", &input) != 1) {
        clear_input_line();
        printf("Invalid input.\n");
        return;
    }
    clear_input_line();
    switch (input) {
        case 1:
            tutorial();
            break;
        case 2:
            // research_tree() will be added later.
            printf("Research tree is not implemented yet.\n");
            break;
        case 3:
            printf("Choose product list from the Company menu for now.\n");
            company();
            break;
        case 4:
            erase_screen();
            launcher();
            break;
        default:
            printf("Invalid input.\n");
            break;
    }
}


void launcher(void) {
    printf("===========================\n");
    printf("  Tech Company Simulator    \n");
    printf("===========================\n");
    printf("[1] Launch simulator\n");
    printf("[2] Help/documentation\n");
    printf("[3] Exit\n");
    printf("===========================\n");
    printf("Input: ");
    if (scanf("%d", &input) != 1) {
        clear_input_line();
        printf("Invalid input.\n");
        return;
    }
    clear_input_line();
    switch (input) {
        case 1:
            printf("Enter your company name: ");
            if (!fgets(company_name, sizeof(company_name), stdin)) company_name[0] = '\0';
            company_name[strcspn(company_name, "\n")] = '\0';
            erase_screen();
            main_menu();
            break;
        case 2:
            erase_screen();
            help_documentation();
            break;
        case 3:
            exit(0);
            break;
        default:
            printf("Invalid input.\n");
            break;
    }
}


void company(void) {
    printf("=======================\n");
    printf("%s stats/products\n", company_name);
    printf("=======================\n");
    printf("[1] View products\n");
    printf("[2] View profit\n");
    printf("[3] View reports/issues\n");
    printf("[4] Return to main menu\n");
    printf("=======================\n");
    printf("Input: ");
    if (scanf("%d", &input) != 1) {
        clear_input_line();
        printf("Invalid input.\n");
        return;
    }
    clear_input_line();
    switch (input) {
        case 1:
            printf("\nWhich product list do you want to view? [1] CPUs [2] GPUs [3] Both: ");
            if (scanf("%d", &input) != 1) {
                clear_input_line();
                printf("Invalid input.\n");
                company();
                return;
            }
            clear_input_line();
            if (input == 1) list_cpus();
            else if (input == 2) list_gpus();
            else if (input == 3) {
                printf("\n--- CPUs ---\n");
                if (cpu_count == 0) printf("No CPU designs yet.\n");
                else {
                    for (int i = 0; i < cpu_count; i++) {
                        printf("CPU #%d: %s | %d cores/%d threads | %.2f GHz base | %.2f GHz overclock | $%d\n",
                               i + 1, cpus[i].name, cpus[i].cores, cpus[i].threads,
                               cpus[i].base_speed, cpus[i].overclock_speed, cpus[i].cost);
                    }
                }
                printf("\n--- GPUs ---\n");
                if (gpu_count == 0) printf("No GPU designs yet.\n");
                else {
                    for (int i = 0; i < gpu_count; i++) {
                        printf("GPU #%d: %s | tier %d | %d compute units | %d GB VRAM | %.0f MHz | $%d\n",
                               i + 1, gpus[i].name, gpus[i].architecture, gpus[i].compute_units,
                               gpus[i].vram, gpus[i].core_clock, gpus[i].cost);
                    }
                }
                printf("\nPress Enter to return...");
                getchar();
                company();
            } else {
                printf("Invalid choice.\n");
                company();
            }
            break;
        case 2:
            printf("Profit tracking is not implemented yet.\n");
            company();
            break;
        case 3:
            printf("Reports/issues are not implemented yet.\n");
            company();
            break;
        case 4:
            erase_screen();
            main_menu();
            break;
        default:
            printf("Invalid input.\n");
            company();
            break;
    }
}


void Sell() {
    char marketname[16];
    double marketprice = 0;

    printf("What do you want to sell [cpu/gpu]? ");

    if (scanf("%49s", charinput) != 1) {
        clear_input_line();
        return;
    }
    clear_input_line();

    if (strcmp(charinput, "cpu") == 0) {

        if (cpu_count == 0) {
            printf("You have not designed any CPUs yet.\n");
            return;
        }

        for (int i = 0; i < cpu_count; i++) {
            printf("\nCPU #%d\n", i + 1);
            printf("Name: %s\n", cpus[i].name);
            printf("Cost: $%d\n", cpus[i].cost);
        }

        printf("\nWhich CPU do you wish to sell (CPU#)? ");

        if (scanf("%d", &input) != 1) {
            clear_input_line();
            printf("Invalid selection.\n");
            return;
        }
        clear_input_line();

        int i = input - 1;

        if (i < 0 || i >= cpu_count) {
            printf("Invalid CPU number.\n");
            return;
        }

        strcpy(marketname, cpus[i].name);
        marketprice = cpus[i].cost * (profitm + 1);

        printf("\nCPU #%d\n", i + 1);
        printf("Name: %s\n", cpus[i].name);
        printf("Cores: %d | Threads: %d\n", cpus[i].cores, cpus[i].threads);
        printf("Base clock: %.2f GHz\n", cpus[i].base_speed);
        printf("Overclock: %.2f GHz\n", cpus[i].overclock_speed);
        printf("Overheat: %.2f%%\n", cpus[i].overheat);
        printf("Market price: $%.2f\n", marketprice);
        printf("===================\n");

    } else if (strcmp(charinput, "gpu") == 0) {

        if (gpu_count == 0) {
            printf("You have not designed any GPUs yet.\n");
            return;
        }

        for (int i = 0; i < gpu_count; i++) {
            printf("\nGPU #%d\n", i + 1);
            printf("Name: %s\n", gpus[i].name);
            printf("Cost: $%d\n", gpus[i].cost);
        }

        printf("\nWhich GPU do you wish to sell (GPU#)? ");

        if (scanf("%d", &input) != 1) {
            clear_input_line();
            printf("Invalid selection.\n");
            return;
        }
        clear_input_line();

        int i = input - 1;

        if (i < 0 || i >= gpu_count) {
            printf("Invalid GPU number.\n");
            return;
        }

        strcpy(marketname, gpus[i].name);
        marketprice = gpus[i].cost * (profitm + 1);

        printf("\nGPU #%d\n", i + 1);
        printf("Name: %s\n", gpus[i].name);
        printf("Architecture tier: %d (efficiency %.2f)\n",
               gpus[i].architecture, gpus[i].architecture_efficiency);
        printf("Compute units: %d | VRAM: %d GB\n",
               gpus[i].compute_units, gpus[i].vram);
        printf("Core clock: %.2f MHz\n", gpus[i].core_clock);
        printf("Max clock: %.2f MHz\n", gpus[i].max_core_clock);
        printf("Cooling rating: %.2f%%\n", gpus[i].cooling);
        printf("TDP: %d W\n", gpus[i].tdp);
        printf("Market price: $%.2f\n", marketprice);
        printf("===================\n");

    } else {
        printf("Please type cpu or gpu.\n");
    }

    printf("\nPress 1 to return to main menu or 2 to sell another product: ");

    if (scanf("%d", &input) != 1) {
        clear_input_line();
        return;
    }
    clear_input_line();

    if (input == 1) {
        erase_screen();
        main_menu();
    } else if (input == 2) {
        erase_screen();
        Sell();
    } else {
        printf("Invalid selection.\n");
    }
}
void customer(){
	
	}

int main(void) {
    if (new_user == 1) {
        launcher();
    }
    return 0;
}
