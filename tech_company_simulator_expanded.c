#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_CPUS 100
#define MAX_GPUS 100
#define MAX_FACTORIES 10
#define MAX_REVIEWS 8
#define SAVE_FILE "tech_company_save.dat"
#define SAVE_VERSION 3

/* =========================================================
   TECH COMPANY SIMULATOR
   A turn-based, text-only technology company management game.
   Save files are written in the folder where the program runs.
   ========================================================= */

typedef struct {
    char name[32];
    int cores;
    int threads;
    double base_speed;
    double boost_speed;
    int cache_mb;
    int process_nm;
    int tdp;
    double overheat;
    int cost;
    int price;
    int stock;
    int sold;
    double rating;
    int review_count;
} CPU;

typedef struct {
    char name[32];
    int architecture;
    double architecture_efficiency;
    int compute_units;
    int vram;
    int memory_bus;
    double core_clock;
    double memory_clock;
    int process_nm;
    int tdp;
    double cooling;
    int cost;
    int price;
    int stock;
    int sold;
    double rating;
    int review_count;
} GPU;

typedef struct {
    char name[32];
    int capacity;
    int level;
    int monthly_cost;
} Factory;

typedef struct {
    char company_name[50];
    double money;
    int month;
    int year;
    int reputation;
    int employees;
    int skill_points;
    int research_points;
    int marketing_level;
    int cpu_lab_level;
    int gpu_lab_level;
    int manufacturing_level;
    int logistics_level;
    int unlocked_architecture;
    int total_sales;
    int total_revenue;
    int total_products;
    int cpu_count;
    int gpu_count;
    int factory_count;
    CPU cpus[MAX_CPUS];
    GPU gpus[MAX_GPUS];
    Factory factories[MAX_FACTORIES];
    int competitor_strength[5];
    int skill_unlocked[10];
    int upgrade_levels[5];
} Game;

Game game;

const char *architecture_names[10] = {
    "Legacy", "Coreline", "Vector", "Pulse", "Nova",
    "Apex", "Titan", "Quantum", "Eclipse", "Omniscale"
};
const double architecture_efficiencies[10] = {
    0.60, 0.80, 1.00, 1.25, 1.55,
    1.90, 2.40, 3.00, 3.80, 5.00
};
const int architecture_unlock_months[10] = {
    1, 2, 4, 6, 9, 12, 16, 20, 25, 30
};

const char *competitor_names[5] = {
    "NexCore", "Voltix", "Quantum Systems", "MicroForge", "TitanTech"
};

void clear_line(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {}
}

int read_int(const char *prompt, int min, int max) {
    char line[128];
    long value;
    char *end;
    for (;;) {
        printf("%s", prompt);
        if (!fgets(line, sizeof(line), stdin)) return min;
        value = strtol(line, &end, 10);
        while (*end == ' ' || *end == '\t') end++;
        if (end == line || (*end != '\n' && *end != '\0') || value < min || value > max) {
            printf("Please enter a number from %d to %d.\n", min, max);
            continue;
        }
        return (int)value;
    }
}

double read_double(const char *prompt, double min, double max) {
    char line[128];
    double value;
    char *end;
    for (;;) {
        printf("%s", prompt);
        if (!fgets(line, sizeof(line), stdin)) return min;
        value = strtod(line, &end);
        while (*end == ' ' || *end == '\t') end++;
        if (end == line || (*end != '\n' && *end != '\0') ||
            !isfinite(value) || value < min || value > max) {
            printf("Please enter a value from %.2f to %.2f.\n", min, max);
            continue;
        }
        return value;
    }
}

void read_text(const char *prompt, char *out, size_t size) {
    printf("%s", prompt);
    if (!fgets(out, (int)size, stdin)) {
        out[0] = '\0';
        return;
    }
    out[strcspn(out, "\n")] = '\0';
    if (out[0] == '\0') snprintf(out, size, "Unnamed");
}

int choose_process_nm(int lab_level) {
    printf("Available manufacturing processes: 14nm, 10nm, 7nm");
    if (game.skill_unlocked[6]) printf(", 5nm");
    if (game.skill_unlocked[6] && lab_level >= 5) printf(", 3nm");
    if (game.skill_unlocked[6] && lab_level >= 8) printf(", 2nm");
    printf("\n");
    for (;;) {
        int nm = read_int("Choose process size in nm: ", 2, 14);
        if (nm == 14 || nm == 10 || nm == 7) return nm;
        if (nm == 5 && game.skill_unlocked[6]) return nm;
        if (nm == 3 && game.skill_unlocked[6] && lab_level >= 5) return nm;
        if (nm == 2 && game.skill_unlocked[6] && lab_level >= 8) return nm;
        printf("That process node is still locked. Check Next-Gen Silicon and your R&D lab level.\n");
    }
}

void pause_enter(void) {
    char line[8];
    printf("\nPress Enter to continue...");
    fgets(line, sizeof(line), stdin);
}

void clear_screen(void) {
    /* Portable fallback: works in terminals without ANSI support too. */
    printf("\n\n");
}

int factory_capacity(void) {
    int total = 0;
    for (int i = 0; i < game.factory_count; i++) {
        total += game.factories[i].capacity * game.factories[i].level;
    }
    return total + game.manufacturing_level * 5 + (game.skill_unlocked[7] ? 10 : 0);
}

int factory_upkeep(void) {
    int total = 0;
    for (int i = 0; i < game.factory_count; i++) {
        total += game.factories[i].monthly_cost * game.factories[i].level;
    }
    return total;
}

void add_starting_factory(void) {
    game.factory_count = 1;
    snprintf(game.factories[0].name, sizeof(game.factories[0].name), "Starter Workshop");
    game.factories[0].capacity = 8;
    game.factories[0].level = 1;
    game.factories[0].monthly_cost = 40;
}

void new_game(void) {
    memset(&game, 0, sizeof(game));
    read_text("Enter your company name: ", game.company_name, sizeof(game.company_name));
    game.money = 1000.0;
    game.month = 1;
    game.year = 2026;
    game.reputation = 50;
    game.employees = 3;
    game.skill_points = 2;
    game.research_points = 0;
    game.unlocked_architecture = 1;
    game.marketing_level = 1;
    game.cpu_lab_level = 1;
    game.gpu_lab_level = 1;
    game.manufacturing_level = 1;
    game.logistics_level = 1;
    for (int i = 0; i < 5; i++) {
        game.competitor_strength[i] = 45 + i * 5;
    }
    add_starting_factory();
    printf("\nWelcome to the industry, %s!\n", game.company_name);
    printf("Your Starter Workshop is ready. Your first challenge: build a product people want!\n");
    pause_enter();
}

int save_game(void) {
    FILE *file = fopen(SAVE_FILE, "wb");
    if (!file) {
        printf("Could not open %s for writing.\n", SAVE_FILE);
        return 0;
    }
    unsigned int magic = 0x54435333; /* TCS3 */
    unsigned int version = SAVE_VERSION;
    int ok = fwrite(&magic, sizeof(magic), 1, file) == 1 &&
             fwrite(&version, sizeof(version), 1, file) == 1 &&
             fwrite(&game, sizeof(game), 1, file) == 1;
    fclose(file);
    if (!ok) {
        printf("Save failed. Check available disk space and permissions.\n");
        return 0;
    }
    printf("Game saved to %s!\n", SAVE_FILE);
    return 1;
}

int load_game(void) {
    FILE *file = fopen(SAVE_FILE, "rb");
    if (!file) return 0;
    unsigned int magic = 0, version = 0;
    int ok = fread(&magic, sizeof(magic), 1, file) == 1 &&
             fread(&version, sizeof(version), 1, file) == 1 &&
             magic == 0x54435333 && version == SAVE_VERSION &&
             fread(&game, sizeof(game), 1, file) == 1;
    fclose(file);
    if (!ok || game.cpu_count < 0 || game.cpu_count > MAX_CPUS ||
        game.gpu_count < 0 || game.gpu_count > MAX_GPUS ||
        game.factory_count < 0 || game.factory_count > MAX_FACTORIES) {
        printf("The save file is damaged or from an incompatible version.\n");
        return 0;
    }
    return 1;
}

double cpu_performance(const CPU *cpu) {
    double clock_score = cpu->base_speed * 18.0 + cpu->boost_speed * 9.0;
    double core_score = cpu->cores * 8.0 + cpu->threads * 2.0;
    double cache_score = cpu->cache_mb * 0.7;
    double process_bonus = 35.0 / cpu->process_nm;
    double heat_penalty = cpu->overheat * 0.18;
    double skill_bonus = game.skill_unlocked[0] ? 1.08 : 1.0;
    return fmax(5.0, (clock_score + core_score + cache_score + process_bonus - heat_penalty)
                * (1.0 + game.cpu_lab_level * 0.04) * skill_bonus);
}

double gpu_performance(const GPU *gpu) {
    double compute = gpu->compute_units * 2.5;
    double memory = gpu->vram * 2.0 + gpu->memory_bus * 0.12;
    double clock = gpu->core_clock * 0.025 + gpu->memory_clock * 0.008;
    double process_bonus = 30.0 / gpu->process_nm;
    double cooling_bonus = gpu->cooling * 0.08;
    double skill_bonus = game.skill_unlocked[1] ? 1.08 : 1.0;
    return fmax(5.0, (compute + memory + clock + process_bonus + cooling_bonus)
                * gpu->architecture_efficiency * (1.0 + game.gpu_lab_level * 0.04) * skill_bonus);
}

void show_company(void) {
    printf("\n========== %s ==========\n", game.company_name);
    printf("Date: Month %d, %d\n", game.month, game.year);
    printf("Cash: $%.2f | Reputation: %d/100\n", game.money, game.reputation);
    printf("Employees: %d | Skill points: %d | Research points: %d\n",
           game.employees, game.skill_points, game.research_points);
    printf("Products: %d | Lifetime sales: %d | Revenue: $%d\n",
           game.total_products, game.total_sales, game.total_revenue);
    printf("Factory capacity: %d units/month\n", factory_capacity());
    printf("====================================\n");
}

void print_cpu(const CPU *c, int number) {
    printf("\nCPU #%d: %s\n", number, c->name);
    printf("Cores/threads: %d/%d | Cache: %d MB\n", c->cores, c->threads, c->cache_mb);
    printf("Clock: %.2f GHz base / %.2f GHz boost\n", c->base_speed, c->boost_speed);
    printf("Process: %dnm | TDP: %dW | Heat risk: %.1f\n", c->process_nm, c->tdp, c->overheat);
    printf("Performance: %.1f | Design cost: $%d | Price: $%d\n",
           cpu_performance(c), c->cost, c->price);
    printf("Stock: %d | Sold: %d | Rating: %.1f/5\n", c->stock, c->sold, c->rating);
}

void print_gpu(const GPU *g, int number) {
    printf("\nGPU #%d: %s\n", number, g->name);
    printf("Architecture: %s (x%.2f)\n",
           architecture_names[g->architecture - 1], g->architecture_efficiency);
    printf("Compute units: %d | VRAM: %dGB | Bus: %dbit\n",
           g->compute_units, g->vram, g->memory_bus);
    printf("Core/memory clock: %.0f/%.0f MHz\n", g->core_clock, g->memory_clock);
    printf("Process: %dnm | TDP: %dW | Cooling: %.0f/100\n",
           g->process_nm, g->tdp, g->cooling);
    printf("Performance: %.1f | Design cost: $%d | Price: $%d\n",
           gpu_performance(g), g->cost, g->price);
    printf("Stock: %d | Sold: %d | Rating: %.1f/5\n", g->stock, g->sold, g->rating);
}

void design_cpu(void) {
    if (game.cpu_count >= MAX_CPUS) {
        printf("Your CPU design database is full.\n");
        return;
    }
    CPU c;
    memset(&c, 0, sizeof(c));
    printf("\n========== CPU DESIGN LAB ==========\n");
    printf("Your R&D lab level: %d\n", game.cpu_lab_level);
    read_text("CPU product name: ", c.name, sizeof(c.name));
    c.cores = read_int("Core count (2-64): ", 2, 64);
    if (c.cores % 2 != 0) {
        printf("For this simulator, core counts must be even. Rounding up to %d.\n", c.cores + 1);
        c.cores++;
    }
    c.threads = c.cores * read_int("Threads per core (1-2): ", 1, 2);
    c.base_speed = read_double("Base clock (1.0-6.0 GHz): ", 1.0, 6.0);
    c.boost_speed = read_double("Boost clock (base clock to 7.0 GHz): ", c.base_speed, 7.0);
    c.cache_mb = read_int("Total cache (4-256 MB): ", 4, 256);
    c.process_nm = choose_process_nm(game.cpu_lab_level);
    c.tdp = (int)(35 + c.cores * 3 + c.boost_speed * 13 + 45.0 / c.process_nm);
    c.overheat = fmax(0.0, (c.boost_speed - c.base_speed) * 22.0 - game.cpu_lab_level * 2.0);
    c.cost = (int)(100 + c.cores * 12 + c.cache_mb * 2 + c.tdp * 1.5 + 500.0 / c.process_nm);
    c.cost = (int)(c.cost * (1.0 - fmin(0.20, game.cpu_lab_level * 0.025)));
    c.price = c.cost + (int)(c.cost * 0.45);
    c.rating = 3.0;
    if (game.money < c.cost) {
        printf("You need $%d to complete this design, but only have $%.2f.\n", c.cost, game.money);
        return;
    }
    game.money -= c.cost;
    game.cpus[game.cpu_count++] = c;
    game.total_products++;
    printf("\nCPU prototype completed!\n");
    print_cpu(&game.cpus[game.cpu_count - 1], game.cpu_count);
    printf("Design cost of $%d deducted. Set its selling price later in Product Management.\n", c.cost);
}

void design_gpu(void) {
    if (game.gpu_count >= MAX_GPUS) {
        printf("Your GPU design database is full.\n");
        return;
    }
    GPU g;
    memset(&g, 0, sizeof(g));
    printf("\n========== GPU DESIGN LAB ==========\n");
    printf("Your GPU R&D lab level: %d\n", game.gpu_lab_level);
    printf("Unlocked architectures:\n");
    for (int i = 0; i < game.unlocked_architecture; i++) {
        printf("%d) %s (efficiency x%.2f)\n", i + 1, architecture_names[i],
               architecture_efficiencies[i]);
    }
    g.architecture = read_int("Choose architecture: ", 1, game.unlocked_architecture);
    g.architecture_efficiency = architecture_efficiencies[g.architecture - 1];
    read_text("GPU product name: ", g.name, sizeof(g.name));
    g.compute_units = read_int("Compute units (8-256): ", 8, 256);
    g.vram = read_int("VRAM (4-48 GB): ", 4, 48);
    printf("Memory bus options: 64, 128, 192, 256, 320, 384, 512 bit\n");
    do {
        g.memory_bus = read_int("Memory bus width: ", 64, 512);
        if (g.memory_bus == 64 || g.memory_bus == 128 || g.memory_bus == 192 ||
            g.memory_bus == 256 || g.memory_bus == 320 || g.memory_bus == 384 ||
            g.memory_bus == 512) break;
        printf("Choose one of the listed bus widths.\n");
    } while (1);
    g.core_clock = read_double("Core clock (500-3000 MHz): ", 500, 3000);
    g.memory_clock = read_double("Memory clock (500-3000 MHz): ", 500, 3000);
    g.process_nm = choose_process_nm(game.gpu_lab_level);
    g.tdp = (int)(60 + g.compute_units * 2 + g.core_clock * 0.10 + g.vram * 4);
    g.cooling = read_int("Cooling quality (40-100): ", 40, 100);
    g.cost = (int)(150 + g.compute_units * 8 + g.vram * 18 + g.memory_bus * 0.8 +
                   g.tdp * 1.2 + 600.0 / g.process_nm);
    g.cost = (int)(g.cost * (1.0 - fmin(0.20, game.gpu_lab_level * 0.025)));
    g.price = g.cost + (int)(g.cost * 0.50);
    g.rating = 3.0;
    if (game.money < g.cost) {
        printf("You need $%d to complete this design, but only have $%.2f.\n", g.cost, game.money);
        return;
    }
    game.money -= g.cost;
    game.gpus[game.gpu_count++] = g;
    game.total_products++;
    printf("\nGPU prototype completed!\n");
    print_gpu(&game.gpus[game.gpu_count - 1], game.gpu_count);
    printf("Design cost of $%d deducted.\n", g.cost);
}

void product_management(void) {
    int type = read_int("\nManage [1] CPU products [2] GPU products [3] Back: ", 1, 3);
    if (type == 3) return;
    if (type == 1) {
        if (!game.cpu_count) { printf("No CPU products yet.\n"); return; }
        for (int i = 0; i < game.cpu_count; i++) print_cpu(&game.cpus[i], i + 1);
        int n = read_int("Select CPU number: ", 1, game.cpu_count) - 1;
        game.cpus[n].price = read_int("New selling price ($1-$100000): ", 1, 100000);
        printf("%s is now listed for $%d.\n", game.cpus[n].name, game.cpus[n].price);
    } else {
        if (!game.gpu_count) { printf("No GPU products yet.\n"); return; }
        for (int i = 0; i < game.gpu_count; i++) print_gpu(&game.gpus[i], i + 1);
        int n = read_int("Select GPU number: ", 1, game.gpu_count) - 1;
        game.gpus[n].price = read_int("New selling price ($1-$200000): ", 1, 200000);
        printf("%s is now listed for $%d.\n", game.gpus[n].name, game.gpus[n].price);
    }
}

void list_products(void) {
    printf("\n========== PRODUCT CATALOG ==========\n");
    if (!game.cpu_count && !game.gpu_count) printf("No products designed yet.\n");
    for (int i = 0; i < game.cpu_count; i++) print_cpu(&game.cpus[i], i + 1);
    for (int i = 0; i < game.gpu_count; i++) print_gpu(&game.gpus[i], i + 1);
}

void customer_comment(const char *name, double rating, int price, int performance) {
    static const char *good[] = {
        "\"Excellent performance for the price!\"",
        "\"A great product. I would buy it again.\"",
        "\"Runs smoothly and feels well engineered.\"",
        "\"This company is making impressive hardware.\""
    };
    static const char *mixed[] = {
        "\"Pretty good, but the price could be better.\"",
        "\"Decent performance. Nothing groundbreaking.\"",
        "\"Works fine, although I expected a little more.\""
    };
    static const char *bad[] = {
        "\"Too expensive for what it offers.\"",
        "\"I expected better performance.\"",
        "\"Not a great value. There are better options.\""
    };
    const char *comment;
    if (rating >= 4.0) comment = good[rand() % 4];
    else if (rating >= 3.0) comment = mixed[rand() % 3];
    else comment = bad[rand() % 3];
    printf("Customer review for %s: %s (%d performance, $%d)\n",
           name, comment, performance, price);
}

double market_demand(double performance, int price, double rating) {
    double value_score = performance * 5.0 / (price > 0 ? price : 1);
    double demand = 2.0 + performance * 0.045 + value_score * 30.0;
    demand *= (0.65 + game.reputation / 150.0);
    demand *= (0.75 + rating / 10.0);
    demand *= (0.85 + game.marketing_level * 0.08);
    if (game.skill_unlocked[3]) demand *= 1.10;
    if (game.skill_unlocked[9]) demand *= 1.08;
    if (game.skill_unlocked[4]) rating += 0.25;
    return fmax(0.0, demand);
}

void simulate_market(void) {
    int capacity = factory_capacity();
    int produced = 0;
    int total_units = game.cpu_count + game.gpu_count;
    if (total_units == 0) {
        printf("No products are listed. Design hardware first.\n");
        return;
    }
    printf("\n========== MARKET REPORT: MONTH %d ==========\n", game.month);
    printf("Competitor activity:\n");
    for (int i = 0; i < 5; i++) {
        int change = rand() % 7 - 3;
        game.competitor_strength[i] += change;
        if (game.competitor_strength[i] < 20) game.competitor_strength[i] = 20;
        if (game.competitor_strength[i] > 95) game.competitor_strength[i] = 95;
        printf(" - %s: market strength %d/100\n", competitor_names[i],
               game.competitor_strength[i]);
    }

    int remaining_capacity = capacity;
    for (int i = 0; i < game.cpu_count; i++) {
        CPU *c = &game.cpus[i];
        int make = remaining_capacity > 0 ? remaining_capacity : 0;
        if (make > 12) make = 12;
        if (make > 0) {
            int unit_cost = c->cost / 8 + 1;
            if (game.skill_unlocked[2]) unit_cost = (int)(unit_cost * 0.95);
            int affordable = (int)(game.money / unit_cost);
            if (make > affordable) make = affordable;
            if (make > 0) {
                game.money -= (double)(make * unit_cost);
                c->stock += make;
                produced += make;
                remaining_capacity -= make;
            }
        }
        double perf = cpu_performance(c);
        double demand = market_demand(perf, c->price, c->rating);
        int sold = (int)(demand / (1.0 + game.competitor_strength[0] / 70.0));
        if (sold > c->stock) sold = c->stock;
        if (c->price <= 0) sold = 0;
        if (sold > 0) {
            int revenue = sold * c->price;
            game.money += revenue;
            game.total_revenue += revenue;
            game.total_sales += sold;
            c->sold += sold;
            c->stock -= sold;
            double customer_rating = 3.0 + (perf / (c->price + 1.0)) * 18.0 -
                                     c->overheat * 0.025;
            if (game.skill_unlocked[4]) customer_rating += 0.25;
            if (customer_rating > 5.0) customer_rating = 5.0;
            if (customer_rating < 1.0) customer_rating = 1.0;
            c->rating = (c->rating * c->review_count + customer_rating) /
                        (c->review_count + 1);
            c->review_count++;
            if (rand() % 100 < 55) customer_comment(c->name, customer_rating, c->price, (int)perf);
            printf("%s CPU: sold %d units for $%d.\n", c->name, sold, revenue);
            if (customer_rating >= 4.0) game.reputation += game.skill_unlocked[8] ? 2 : 1;
            else if (customer_rating < 2.8) game.reputation--;
        } else {
            printf("%s CPU: no sales this month. Check its price and performance.\n", c->name);
        }
    }

    for (int i = 0; i < game.gpu_count; i++) {
        GPU *g = &game.gpus[i];
        int make = remaining_capacity > 0 ? remaining_capacity : 0;
        if (make > 10) make = 10;
        if (make > 0) {
            int unit_cost = g->cost / 7 + 1;
            if (game.skill_unlocked[2]) unit_cost = (int)(unit_cost * 0.95);
            int affordable = (int)(game.money / unit_cost);
            if (make > affordable) make = affordable;
            if (make > 0) {
                game.money -= (double)(make * unit_cost);
                g->stock += make;
                produced += make;
                remaining_capacity -= make;
            }
        }
        double perf = gpu_performance(g);
        double demand = market_demand(perf, g->price, g->rating);
        int sold = (int)(demand / (1.0 + game.competitor_strength[1] / 75.0));
        if (sold > g->stock) sold = g->stock;
        if (g->price <= 0) sold = 0;
        if (sold > 0) {
            int revenue = sold * g->price;
            game.money += revenue;
            game.total_revenue += revenue;
            game.total_sales += sold;
            g->sold += sold;
            g->stock -= sold;
            double customer_rating = 3.0 + (perf / (g->price + 1.0)) * 20.0 -
                                     (g->tdp > 350 ? 0.5 : 0.0) +
                                     (g->cooling - 60.0) * 0.015;
            if (game.skill_unlocked[4]) customer_rating += 0.25;
            if (game.skill_unlocked[5]) customer_rating += 0.30;
            if (customer_rating > 5.0) customer_rating = 5.0;
            if (customer_rating < 1.0) customer_rating = 1.0;
            g->rating = (g->rating * g->review_count + customer_rating) /
                        (g->review_count + 1);
            g->review_count++;
            if (rand() % 100 < 55) customer_comment(g->name, customer_rating, g->price, (int)perf);
            printf("%s GPU: sold %d units for $%d.\n", g->name, sold, revenue);
            if (customer_rating >= 4.0) game.reputation += game.skill_unlocked[8] ? 2 : 1;
            else if (customer_rating < 2.8) game.reputation--;
        } else {
            printf("%s GPU: no sales this month. Check its price and performance.\n", g->name);
        }
    }

    if (game.reputation < 0) game.reputation = 0;
    if (game.reputation > 100) game.reputation = 100;
    printf("\nProduction this month: %d units (capacity %d).\n", produced, capacity);
    printf("Market reputation: %d/100\n", game.reputation);
}

void skill_tree(void) {
    const char *names[10] = {
        "CPU Architecture", "GPU Architecture", "Efficient Manufacturing",
        "Marketing Basics", "Customer Research", "Advanced Cooling",
        "Next-Gen Silicon", "Factory Automation", "Brand Loyalty", "Market Analytics"
    };
    const char *desc[10] = {
        "CPU performance +8%.", "GPU performance +8%.",
        "Monthly production costs -5%.", "Marketing effectiveness +10%.",
        "Customer ratings improve slightly.", "High-power GPUs get better ratings.",
        "Unlocks smaller process design options in future updates.",
        "Factory capacity +10 units/month.", "Reputation gains are stronger.",
        "Demand forecasting bonus."
    };
    printf("\n========== COMPANY SKILL TREE ==========\n");
    printf("Skill points available: %d\n", game.skill_points);
    for (int i = 0; i < 10; i++) {
        printf("%2d) %-27s %s %s\n", i + 1, names[i],
               game.skill_unlocked[i] ? "[UNLOCKED]" : "[LOCKED]", desc[i]);
    }
    printf("0) Back\n");
    int choice = read_int("Unlock which skill? ", 0, 10);
    if (choice == 0) return;
    int index = choice - 1;
    if (game.skill_unlocked[index]) {
        printf("You already unlocked that skill.\n");
        return;
    }
    if (game.skill_points < 1) {
        printf("You need at least 1 skill point. Advance months and grow your company to earn more.\n");
        return;
    }
    if (index == 6 && !game.skill_unlocked[0]) {
        printf("Prerequisite: unlock CPU Architecture first.\n");
        return;
    }
    if (index == 7 && !game.skill_unlocked[2]) {
        printf("Prerequisite: unlock Efficient Manufacturing first.\n");
        return;
    }
    if (index == 8 && !game.skill_unlocked[4]) {
        printf("Prerequisite: unlock Customer Research first.\n");
        return;
    }
    game.skill_unlocked[index] = 1;
    game.skill_points--;
    printf("Skill unlocked: %s!\n", names[index]);
}

void upgrade_menu(void) {
    printf("\n========== COMPANY UPGRADES ==========\n");
    printf("1) CPU R&D Lab       Level %d | Cost $%d\n", game.cpu_lab_level,
           game.cpu_lab_level * 700);
    printf("2) GPU R&D Lab       Level %d | Cost $%d\n", game.gpu_lab_level,
           game.gpu_lab_level * 700);
    printf("3) Marketing Team    Level %d | Cost $%d\n", game.marketing_level,
           game.marketing_level * 600);
    printf("4) Manufacturing     Level %d | Cost $%d\n", game.manufacturing_level,
           game.manufacturing_level * 800);
    printf("5) Logistics         Level %d | Cost $%d\n", game.logistics_level,
           game.logistics_level * 500);
    printf("0) Back\n");
    int choice = read_int("Choose upgrade: ", 0, 5);
    if (!choice) return;
    int *level = NULL;
    switch (choice) {
        case 1: level = &game.cpu_lab_level; break;
        case 2: level = &game.gpu_lab_level; break;
        case 3: level = &game.marketing_level; break;
        case 4: level = &game.manufacturing_level; break;
        case 5: level = &game.logistics_level; break;
    }
    int cost = (*level) * (choice == 4 ? 800 : choice == 3 ? 600 : 500);
    if (*level >= 10) {
        printf("This upgrade is already at maximum level 10.\n");
        return;
    }
    if (game.money < cost) {
        printf("You need $%d, but have only $%.2f.\n", cost, game.money);
        return;
    }
    game.money -= cost;
    (*level)++;
    printf("Upgrade purchased! New level: %d.\n", *level);
}

void factory_menu(void) {
    printf("\n========== FACTORY MANAGEMENT ==========\n");
    printf("Factories: %d/%d | Total capacity: %d units/month\n",
           game.factory_count, MAX_FACTORIES, factory_capacity());
    for (int i = 0; i < game.factory_count; i++) {
        Factory *f = &game.factories[i];
        printf("%d) %s | Level %d | Capacity %d | Upkeep $%d/month\n",
               i + 1, f->name, f->level, f->capacity * f->level,
               f->monthly_cost * f->level);
    }
    printf("\n1) Build small factory ($2500)\n");
    printf("2) Build medium factory ($9000)\n");
    printf("3) Upgrade selected factory\n");
    printf("0) Back\n");
    int choice = read_int("Choose: ", 0, 3);
    if (choice == 0) return;
    if (choice == 1 || choice == 2) {
        int price = choice == 1 ? 2500 : 9000;
        if (game.factory_count >= MAX_FACTORIES) {
            printf("Factory limit reached.\n");
            return;
        }
        if (game.money < price) {
            printf("You need $%d for this factory.\n", price);
            return;
        }
        Factory *f = &game.factories[game.factory_count++];
        if (choice == 1) {
            snprintf(f->name, sizeof(f->name), "Small Factory %d", game.factory_count);
            f->capacity = 20;
            f->monthly_cost = 120;
        } else {
            snprintf(f->name, sizeof(f->name), "Medium Factory %d", game.factory_count);
            f->capacity = 70;
            f->monthly_cost = 400;
        }
        f->level = 1;
        game.money -= price;
        printf("%s constructed!\n", f->name);
    } else if (choice == 3) {
        if (!game.factory_count) return;
        int n = read_int("Factory number to upgrade: ", 1, game.factory_count) - 1;
        int price = game.factories[n].level * 1500;
        if (game.money < price) {
            printf("You need $%d to upgrade it.\n", price);
            return;
        }
        game.money -= price;
        game.factories[n].level++;
        printf("%s upgraded to level %d!\n",
               game.factories[n].name, game.factories[n].level);
    }
}

void hire_employees(void) {
    int count = read_int("How many employees to hire (1-20)? ", 1, 20);
    int cost = count * 250;
    if (game.money < cost) {
        printf("Hiring them costs $%d. You only have $%.2f.\n", cost, game.money);
        return;
    }
    game.money -= cost;
    game.employees += count;
    printf("Hired %d employees. Total employees: %d.\n", count, game.employees);
}

void research_menu(void) {
    printf("\n========== RESEARCH & DEVELOPMENT ==========\n");
    printf("Research points: %d\n", game.research_points);
    printf("Next architecture unlock: ");
    if (game.unlocked_architecture < 10) {
        printf("%s at month %d\n", architecture_names[game.unlocked_architecture],
               architecture_unlock_months[game.unlocked_architecture]);
    } else printf("All architectures unlocked!\n");
    printf("1) Invest $500 for 5 research points\n");
    printf("2) View architecture tree\n");
    printf("3) Research next architecture (cost depends on tier)\n");
    printf("0) Back\n");
    int choice = read_int("Choose: ", 0, 3);
    if (choice == 1) {
        if (game.money < 500) {
            printf("Not enough money.\n");
            return;
        }
        game.money -= 500;
        game.research_points += 5;
        printf("Research team gained 5 research points.\n");
    } else if (choice == 2) {
        for (int i = 0; i < 10; i++) {
            printf("%2d) %-12s Efficiency x%.2f | Unlock month %d %s\n",
                   i + 1, architecture_names[i], architecture_efficiencies[i],
                   architecture_unlock_months[i],
                   (i < game.unlocked_architecture) ? "[AVAILABLE]" : "[LOCKED]");
        }
    } else if (choice == 3) {
        if (game.unlocked_architecture >= 10) {
            printf("All architectures are already unlocked.\n");
        } else {
            int cost = game.unlocked_architecture * 5;
            printf("Researching %s costs %d research points.\n",
                   architecture_names[game.unlocked_architecture], cost);
            if (game.research_points < cost) {
                printf("Not enough research points yet.\n");
            } else {
                game.research_points -= cost;
                game.unlocked_architecture++;
                printf("Breakthrough! %s architecture is now available.\n",
                       architecture_names[game.unlocked_architecture - 1]);
            }
        }
    }
}

void advance_month(void) {
    printf("\nAdvancing to the next month...\n");
    simulate_market();
    int wages = game.employees * 35;
    int upkeep = factory_upkeep();
    int operating = wages + upkeep;
    game.money -= operating;
    printf("\nOperating expenses: wages $%d + factory upkeep $%d = $%d.\n",
           wages, upkeep, operating);
    game.research_points += 1 + game.research_points / 20;
    game.skill_points += 1;
    game.month++;
    if (game.month > 12) {
        game.month = 1;
        game.year++;
    }
    if (game.unlocked_architecture < 10 &&
        (game.year - 2026) * 12 + game.month >= architecture_unlock_months[game.unlocked_architecture]) {
        game.unlocked_architecture++;
        printf("\nTECH BREAKTHROUGH! New GPU architecture unlocked: %s!\n",
               architecture_names[game.unlocked_architecture - 1]);
    }
    if (game.skill_unlocked[2]) {
        /* Efficient Manufacturing reduces future upkeep slightly. */
        game.money += 0.0;
    }
    if (game.money < 0) {
        game.reputation -= 3;
        printf("WARNING: Your company is in debt! Reputation has fallen.\n");
    }
    if (game.reputation < 0) game.reputation = 0;
    printf("\nMonth complete. You earned 1 skill point and research progress.\n");
    show_company();
}

void reports_menu(void) {
    printf("\n========== BUSINESS REPORTS ==========\n");
    printf("Lifetime units sold: %d\n", game.total_sales);
    printf("Lifetime revenue: $%d\n", game.total_revenue);
    printf("Current cash: $%.2f\n", game.money);
    printf("Reputation: %d/100\n", game.reputation);
    printf("Monthly factory capacity: %d\n", factory_capacity());
    printf("Monthly factory upkeep: $%d\n", factory_upkeep());
    printf("\nCPU report:\n");
    for (int i = 0; i < game.cpu_count; i++) {
        CPU *c = &game.cpus[i];
        printf("%s: sold %d, stock %d, rating %.1f/5\n",
               c->name, c->sold, c->stock, c->rating);
    }
    printf("\nGPU report:\n");
    for (int i = 0; i < game.gpu_count; i++) {
        GPU *g = &game.gpus[i];
        printf("%s: sold %d, stock %d, rating %.1f/5\n",
               g->name, g->sold, g->stock, g->rating);
    }
}

void main_menu(void) {
    int running = 1;
    while (running) {
        show_company();
        printf("1) Design CPU\n");
        printf("2) Design GPU\n");
        printf("3) Product catalog / set prices\n");
        printf("4) Simulated market: advance month\n");
        printf("5) Skill tree\n");
        printf("6) Company upgrades\n");
        printf("7) Factories\n");
        printf("8) Hire employees\n");
        printf("9) Research tree\n");
        printf("10) Business reports\n");
        printf("11) Save game\n");
        printf("12) Save and exit\n");
        printf("0) Exit to title screen\n");
        int choice = read_int("Command: ", 0, 12);
        switch (choice) {
            case 1: design_cpu(); break;
            case 2: design_gpu(); break;
            case 3: product_management(); list_products(); break;
            case 4: advance_month(); break;
            case 5: skill_tree(); break;
            case 6: upgrade_menu(); break;
            case 7: factory_menu(); break;
            case 8: hire_employees(); break;
            case 9: research_menu(); break;
            case 10: reports_menu(); break;
            case 11: save_game(); break;
            case 12: save_game(); running = 0; break;
            case 0: running = 0; break;
        }
        if (running) pause_enter();
    }
}

int main(void) {
    srand((unsigned int)time(NULL));
    printf("========================================\n");
    printf("       TECH COMPANY SIMULATOR\n");
    printf("========================================\n");
    printf("1) New company\n");
    printf("2) Load saved company\n");
    printf("3) Exit\n");
    int choice = read_int("Choose: ", 1, 3);
    if (choice == 3) return 0;
    if (choice == 2) {
        if (load_game()) {
            printf("Welcome back, %s!\n", game.company_name);
        } else {
            printf("No valid save found. Starting a new company instead.\n");
            new_game();
        }
    } else {
        new_game();
    }
    main_menu();
    printf("\nThanks for playing Tech Company Simulator!\n");
    return 0;
}
