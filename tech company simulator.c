#include <stdio.h>
#include <math.h>
#include <string.h>

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

void FirstPart() {
    tutorialpart++;
    printf("\ntutorial [%d/5]\n", tutorialpart);
    printf("Time to make your first part! First we will make a CPU (central processing unit).\n");
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
            cost += 250;
            break;
        } else if (int_answer == 8) {
            cores = 8;
            cost += 500;
            break;
        } else {
            printf("You did not select a valid number of cores [remember: answer 2, 4, or 8].\n");
        }
    } while (1);

    printf("Selected: %d cores | Cost: $%d\n", cores, cost);

    tutorialpart++;
    printf("\ntutorial [%d/5]\n", tutorialpart);
    printf("Now based on the amount of cores in your CPU you will have double the amount of threads. So 2 cores = 4 threads (+$50/unit), 4 cores = 8 threads (+$150/unit), and 8 cores = 16 threads (+$300/unit).\n");
    
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

    tutorialpart++;
    printf("\ntutorial [%d/5]\n", tutorialpart);
    printf("Now it is time to decide your overclock speed. Be warned: if you set it too high your CPUs will gain a tendency to fry themselves and you'll lose profit and buyers.\n");

    speed = 2.8 + (cores * 0.25);

    printf("Your base speed is %.2f GHz. What will you set the max overclock to?\n", speed);
    printf("Enter overclock speed (GHz): ");
    scanf("%lf", &o_clock_speed);
    while (getchar() != '\n');

    tutorialpart++;
    printf("\ntutorial [%d/5]\n", tutorialpart);
    printf("Now it is time to name your CPU (max of 10 characters)\n");
    printf("Enter CPU name: ");
    fgets(cpu_name, sizeof(cpu_name), stdin);
    if (strchr(cpu_name, '\n') == NULL) {
        while (getchar() != '\n');
    } else {
        cpu_name[strcspn(cpu_name, "\n")] = '\0';
    }

    printf("\n%s has %d cores, %d threads, and a %.2lf GHz base speed with a possibility of %.2lf GHz overclocking, costing $%d. COMING TO MARKET in 5!\n",
           cpu_name, cores, threads, speed, o_clock_speed, cost);
}

int main() {
    if (new_user == 1) {
        tutorialpart = 1;
        printf("Hello, welcome to Tech Company Simulator! What is your company called?\n");
        printf("Enter company name (max 49 characters): ");
        fgets(company_name, sizeof(company_name), stdin);
        company_name[strcspn(company_name, "\n")] = '\0';
        FirstPart();
    }

    return 0;
}
