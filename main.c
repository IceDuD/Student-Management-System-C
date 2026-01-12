/*
  项目名称: 学生信息管理系统
  开发环境: Windows 10 / Dev C++
  组长：郑永炫	20245080905067
  组员：黄子淳	20245080905051
  		陈浩桐	20245080905093
  		罗桂松  20245080905055
		王智斌  20245080905073
  功能描述： 
  1. 数据保存：采用字段级二进制存储（data.txt），确保长学号与特殊成绩数据的精确性与安全性。
  2. Excel交互：支持 CSV 格式导出与智能导入，实现系统数据与 Excel 的无缝对接。
  3. 核心管理：基于单链表结构实现学生信息的录入、指定位置插入、精确/模糊修改与物理删除。
  4. 数据检索：支持按学号精确匹配及按姓名模糊搜索（关键字匹配）。
  5. 数据分析：提供按总分降序、录入时间顺序排列，并能统计全班平均分及极值情况。
*/

#include <stdio.h>
#include <stdlib.h>		// 提供内存管理
#include <string.h>		// 提供字符串操作
#include <time.h>		// 提供时间转换与格式化功能
#include <conio.h>		// 提供非缓冲输入：getch();
#include <windows.h> 	// 提供 Windows 控制台配置

// ================= 宏定义与结构体 =================
#define FILENAME "data.txt"															// 宏定义文件名 
#define FORMAT "%-20s%-15s%-8.1lf%-8.1lf%-8.1lf%-8.1lf%-20s\n"						// 定义控制台输出格式串 
#define DATA p->num, p->name, p->chinese, p->math, p->english, p->sum, p->timestr	// 将学生结构体中的各字段与 FORMAT 格式一一对应，简化 printf 编写


/*
 * 结构名称：student (别名 Student)
 * 功能描述：用于存储单个学生的完整档案。
 * 数据结构：单链表节点。除了存储基本信息外，还包含一个指向下一节点的指针。
 * 创新特色：支持超长学号存储，并包含时间戳用于精确的录入顺序追踪。
 * 作者：郑永炫 
 * 学号：20245080905067
 */
struct student {
    char num[30];           // 学号
    char name[15];          // 姓名
    double chinese;         // 语文成绩
    double math;			// 数学成绩 
    double english;			// 英语成绩 
    double sum;             // 总分
    char timestr[35];       // 录入/修改时间字符串
    long timeStamp;         // 用时间戳（排序） 
    struct student *next;	// 指针域：指向链表中的下一个学生节点
};
typedef struct student Student;		//将 struct student 简化为 Student，方便后续代码编写

// 全局头指针 
Student *head = NULL;							 //指向学生链表的首个节点，是程序的静态起点

// ================= 函数声明 =================
void menu();                                     // 显示主菜单界面 
void readData();                                 // 从文件读取数据 
void saveData();                                 // 保存数据到文件 
void wait_for_enter();                           // 暂停等待用户按键 
void get_current_time(char *tstr, long *tstamp); // 获取并格式化当前时间 
void swap_data(Student *a, Student *b);          // 交换两个学生的数据 

// 核心功能
void input();                                    // 录入学生信息 
void display();                                  // 显示所有学生列表 
void delete_stu();                               // 删除学生信息 
void insert();                                   // 在指定位置插入学生 
void modify();                                   // 修改学生信息 
void statistics();                               // 统计学生成绩数据 

// 查询模块
void search();                                   // 查询功能入口 
void search_by_id();                             // 按学号精确查询 
void search_fuzzy();                             // 按姓名模糊查询 

// 排序模块
void sort();                                     // 排序功能入口 
void sort_by_sum();                              // 按总分高低排序 
void sort_by_time();                             // 按录入时间排序 

//拓展功能！ 
void exportToCSV();								 // 导出到 CSV (可以用 Excel 打开)
void importFromCSV();							 // 从 CSV导入 (从 Excel 编辑后导回程序)


/*
 * 函数名称：main
 * 功能描述：
 *		程序的入口与调度中心。负责初始化控制台界面设置、加载本地二进制数据，
 * 		并进入主程序循环。通过调用菜单函数（menu）引导用户，利用 switch-case 
 * 		分支结构实现对各业务功能模块（如增、删、改、查、Excel导出等）的统一分发与导航。
 * 		同时确保程序在正常退出前完成数据的自动存盘。
 * 输入参数：无
 * 返回值：int - 返回 0 代表程序安全且正常地结束运行
 * 作者：郑永炫 
 * 学号：20245080905067
 */
int main() {
    system("color 0B"); 	//美观：修改字体颜色 

    readData();				// 从文件读取数据 

    int choice;				// 定义变量，用于存储用户输入的菜单选项数字
    while (1) {
        menu();				// 调用菜单函数，在屏幕上打印出菜单栏 
        printf("\n请选择功能(0-10): ");
        if (scanf("%d", &choice) != 1) {	// 一旦用户输入的不是数字，会执行下面的指令回到菜单 
            while(getchar() != '\n'); 		// 清空输入缓冲区防止死循环
            continue;						// 跳过switch 分支，直接回到循环开头重新显示菜单
        }

        switch (choice) {
            case 1: input(); break;
            case 2: search(); break;
            case 3: delete_stu(); break;
            case 4: modify(); break;
            case 5: insert(); break;
            case 6: sort(); break;
            case 7: statistics(); break;
            case 8: display(); break;
            case 9: exportToCSV(); wait_for_enter(); break;
			case 10: importFromCSV(); wait_for_enter(); break;
            case 0: 
                saveData(); 
                printf("\n[提示] 数据已成功存盘，系统退出。\n");
                return 0;
            default: 
                printf("\n[错误] 无此选项，请重新输入！\n");
                wait_for_enter();
        }
    }
    return 0;
}


// ================= 界面模块 =================
/*
 * 函数名称：menu 
 * 功能描述：
 * 		主菜单程序，将菜单界面单独集成作一个函数 
 * 		该函数确保了页面的刷新和菜单的展示。
 * 输入参数：无
 * 返回值：void - 无返回值
 * 作者：郑永炫 
 * 学号：20245080905067
 */
void menu() {
    system("cls");
    printf("\n\n\t\t------------------- 学生信息管理系统 -------------------\n");
    printf("\t\t\t\t1. 录入学生信息\n");
    printf("\t\t\t\t2. 查询学生信息 (子菜单)\n");
    printf("\t\t\t\t3. 删除学生信息\n");
    printf("\t\t\t\t4. 修改学生信息\n");
    printf("\t\t\t\t5. 插入学生信息\n");
    printf("\t\t\t\t6. 排序学生信息 (子菜单)\n");
    printf("\t\t\t\t7. 统计报表\n");
    printf("\t\t\t\t8. 显示所有学生\n");
    printf("\t\t\t\t9. 导出数据到 Excel (CSV)\n");
	printf("\t\t\t\t10.从 Excel (CSV)导入数据\n");
    printf("\t\t\t\t0. 退出并保存\n");
    printf("\t\t--------------------------------------------------------\n");
}

/*
 * 函数名称：wait_for_enter
 * 功能描述：
 * 		交互控制函数。用于在控制台输出信息后强制程序暂停。
 * 		利用 conio.h 库中的 getch() 函数监听键盘动作，实现 “按任意键继续”的功能。
 * 		确保用户有充足的时间阅读屏幕上的信息，
 * 		防止程序立即清屏或跳转回主菜单。
 * 输入参数：无
 * 返回值：void - 无返回值
 * 作者：郑永炫 
 * 学号：20245080905067
 */
void wait_for_enter() {
    printf("\n按任意键返回..."); 		// 
    getch(); 							// 监听键盘输入，不等待回车，实现即时响应
}


// ================= 文件操作 =================

/*
 * 函数名称：saveData
 * 功能描述：
 * 		数据持久化函数。采用二进制写入模式（wb）打开指定文件。
 * 		通过遍历单链表，将内存中每一个学生节点的结构体字段（学号、姓名、
 * 		成绩、时间戳等）按固定字节长度顺序写入磁盘。
 * 		该函数确保了程序关闭后，学生数据仍能安全存储在 data.txt 中。
 * 输入参数：无（直接访问全局头指针）
 * 返回值：void - 无返回值
 * 作者：郑永炫 
 * 学号：20245080905067
 */
void saveData() {
    // 
    FILE *fp = fopen(FILENAME, "wb");           // 以二进制写入模式打开文件
    if (!fp) return;                            // 若打开失败（如文件被占用），则退出
    
    Student *p = head;                          // 定义临时指针 p 从链表头开始
    while (p) {                                 // 循环遍历整个链表
        // 逐项将数据存入磁盘
        fwrite(p->num, 30, 1, fp);              // 写入学号（固定30字节）
        fwrite(p->name, 15, 1, fp);             // 写入姓名（固定15字节）
        fwrite(&p->chinese, sizeof(double), 1, fp); // 写入语文成绩
        fwrite(&p->math, sizeof(double), 1, fp);    // 写入数学成绩
        fwrite(&p->english, sizeof(double), 1, fp); // 写入英语成绩
        fwrite(&p->sum, sizeof(double), 1, fp);     // 写入总分
        fwrite(p->timestr, 35, 1, fp);              // 写入时间字符串
        fwrite(&p->timeStamp, sizeof(long), 1, fp); // 写入排序时间戳
        
        p = p->next;                            // 移动到下一个学生节点
    }
    fclose(fp);                                 // 核心操作：关闭文件，释放资源
}

/*
 * 函数名称：readData
 * 功能描述：
 * 		数据加载函数。程序启动时自动调用，负责将硬盘 data.txt 中的二进制数据
 * 		重新恢复到内存的单链表中。采用尾插法（Tail Insertion）重建链表，
 * 		确保读取后的数据顺序与文件存储顺序保持完全一致。
 * 输入参数：无
 * 返回值：void - 无返回值
 * 作者：郑永炫 
 * 学号：20245080905067
 */
void readData() {
    
    FILE *fp = fopen(FILENAME, "rb");		// 以 "rb" 模式打开文件。r (read) 代表读取，b (binary) 代表二进制。
    if (!fp) return; 						// 如果文件不存在，fopen = 0 -> !fp = 1 直接结束函数（文件不存在或无法打开）

    while (1) { 
        Student *n = (Student *)malloc(sizeof(Student));	// 为准备新读入的学生申请内存空间
        
        if (fread(n->num, 30, 1, fp) != 1) { 				// 读取 1次 30个字节（学号），如果读取到了返回 1，如果没有读取到则返回 0  
            free(n); 										// (0 != 1)值为 1，清除用不到的内存n 
            break; 											// 退出while循环停止读取txt 
        }

        // 成功读取到学号之后，读取完该学号的所有学生信息 
        fread(n->name, 15, 1, fp);							//读取 15字节（名字）
        fread(&n->chinese, sizeof(double), 1, fp);			//读取 double 字节（中文成绩）
        fread(&n->math, sizeof(double), 1, fp);				//读取 double 字节（数学成绩）
        fread(&n->english, sizeof(double), 1, fp);			//读取 double 字节（英文成绩）
        fread(&n->sum, sizeof(double), 1, fp);				//读取 double 字节（总成绩）
        fread(n->timestr, 35, 1, fp);						//读取 35 字节（时间文本信息）
        fread(&n->timeStamp, sizeof(long), 1, fp);			//读取 long 字节（时间戳数据）
        
        n->next = NULL; 					// 确保每次输入后 n->next为空，这是链表的终点 

        // 尾插法，下面是整理链表 
        if (!head) {						// head 为空（说明这是读到的第一个人）
            head = n; 						// 如果是第一个读到的，把 head 指向结点 n 
        } else {							// else 如果不是第一个找到的 
            Student *p = head;				// 创建临时指针 p从头出发 
            while (p->next) p = p->next; 	// 找到当前链表的末尾
            p->next = n; 					// 连接新节点
        }
    }

    // 关闭文件流
    fclose(fp);
}


// ================= 核心功能逻辑 =================

/*
 * 函数名称：input
 * 功能描述：
 *		学生信息录入功能。通过控制台交互引导用户输入学生信息（学号、姓名、三科成绩），
 *		在录入过程中实时进行学号查重验证，防止重复录入。使用尾插法将新学生节点插入链表，
 *		自动计算总分，记录录入时间，并实时保存到数据文件。支持连续录入模式，用户可控制
 *		是否继续添加下一条记录，确保数据完整性和一致性。
 * 输入参数：无
 * 返回值：void
 * 作者：罗桂松 
 * 学号：20245080905055
 */
void input() {
    char ch[2];
    do {
        Student *n = (Student *)malloc(sizeof(Student));
        printf("\n请输入学号: "); scanf("%s", n->num);
        
        // 1. 查重逻辑保持不变
        Student *p = head; int exist = 0;
        while(p) {
            if(strcmp(p->num, n->num) == 0) { exist = 1; break; }
            p = p->next;
        }

        if(exist) { 
            printf("错误：该学号已存在！\n"); 
            free(n);
        }
        else {
            printf("请输入姓名: "); scanf("%s", n->name);
            
            // 语文成绩校验
            do {
                printf("语文 (0-150): "); scanf("%lf", &n->chinese);
                if (n->chinese < 0 || n->chinese > 150) 
                    printf("【输入错误】成绩必须在 0 到 150 之间，请重新输入！\n");
            } while (n->chinese < 0 || n->chinese > 150);

            // 数学成绩校验
            do {
                printf("数学 (0-150): "); scanf("%lf", &n->math);
                if (n->math < 0 || n->math > 150) 
                    printf("【输入错误】成绩必须在 0 到 150 之间，请重新输入！\n");
            } while (n->math < 0 || n->math > 150);

            // 英语成绩校验
            do {
                printf("英语 (0-150): "); scanf("%lf", &n->english);
                if (n->english < 0 || n->english > 150) 
                    printf("【输入错误】成绩必须在 0 到 150 之间，请重新输入！\n");
            } while (n->english < 0 || n->english > 150);

            n->sum = n->chinese + n->math + n->english;
            get_current_time(n->timestr, &n->timeStamp);
            n->next = NULL; 

            // 2. 尾插法逻辑保持不变
            if (head == NULL) {
                head = n;
            } else {
                Student *temp = head;
                while (temp->next != NULL) temp = temp->next;
                temp->next = n;
            }

            printf("录入成功！\n");
            saveData();
        }
        printf("继续录入吗？(y/n): "); scanf("%s", ch);
    } while (strcmp(ch, "y") == 0 || strcmp(ch, "Y") == 0);
}

/*
 * 函数名称：display
 * 功能描述：
 *		显示所有学生信息功能。首先清空控制台屏幕，然后检查学生数据链表是否为空。
 *		若链表为空，提示用户暂无数据并返回；若链表非空，按照固定格式打印表头（包括
 *		学号、姓名、三科成绩、总分和最后操作时间），随后遍历整个链表，逐行输出每个
 *		学生的详细信息。所有信息以对齐的表格形式呈现，确保可读性。显示完成后等待
 *		用户按下回车键返回上级菜单。
 * 输入参数：无
 * 返回值：void
 * 作者：罗桂松 
 * 学号：20245080905055
 */
void display() {
    system("cls");																// 清屏，让界面更清晰
    
    // 检查链表是否为空
    if (!head) { printf("\n暂无学生数据。\n"); wait_for_enter(); return; }		// head为NULL表示链表为空;等待用户按回车;结束函数
    
    // 打印表头
    // %-20s 表示左对齐，占20个字符宽度
	printf("\n%-20s%-15s%-8s%-8s%-8s%-8s%-20s\n", "学号", "姓名", "语文", "数学", "英语", "总分", "最后操作时间");
    printf("------------------------------------------------------------------------------------------\n");
 	// 遍历链表并打印每个学生的信息   
    Student *p = head;								 							// 从头节点开始
    while (p) { printf(FORMAT, DATA); p = p->next; } 	 						// 当p不为NULL时继续循环
    wait_for_enter();
}


// ================= 查询子模块 (增加子菜单和模糊查询) =================

/*
 * 函数名称：search
 * 功能描述：
 *      提供查询功能的子菜单界面，作为查询操作的入口函数。负责清空控制台界面，
 *      打印查询子菜单选项（按学号精确查询、按姓名模糊查询、返回主菜单），
 *      接收用户的菜单选择并分发到对应的查询功能函数执行，执行完成后等待用户
 *      按回车键，避免操作结果界面快速消失。
 * 输入参数：无
 * 返回值：void - 无返回值
 * 作者：陈浩桐 
 * 学号：20245080905093
 */
void search() {
    int sel;                                            // 定义变量sel，用于存储用户的菜单选择
    system("cls");                                      // 清屏操作，清空控制台界面
    printf("\n======= 查询子菜单 =======\n");           // 打印查询子菜单标题
    printf("1. 按学号精确查询\n");                      // 打印选项1：按学号精确查询
    printf("2. 按姓名模糊查询\n");                      // 打印选项2：按姓名模糊查询
    printf("0. 返回主菜单\n");                          // 打印选项0：返回主菜单
    printf("==========================\n请选择: ");     // 打印菜单分隔线，并提示用户输入选择
    scanf("%d", &sel);                                  // 读取用户输入的选择数字，存入sel变量
    if (sel == 1) search_by_id();                       // 如果选择1，调用按学号查询函数
    else if (sel == 2) search_fuzzy();                  // 如果选择2，调用按姓名模糊查询函数
    else return;                                        // 选择其他（如0），直接返回，退出当前函数
    wait_for_enter();                                   // 等待用户按回车键（自定义函数），防止界面一闪而过
}

/*
 * 函数名称：search_by_id
 * 功能描述：
 *      实现按学号精确查询学生信息的功能。接收用户输入的目标学号，遍历存储学生信息的链表，
 *      通过字符串比对（strcmp）精准匹配学号，若找到匹配的学生节点，则按指定格式打印该学生
 *      的学号、姓名、语文/数学/英语成绩及总分；若遍历完链表未找到匹配项，则提示无该学号记录。
 * 输入参数：无
 * 返回值：void - 无返回值
 * 作者：陈浩桐 
 * 学号：20245080905093
 */
void search_by_id() {                        // 按学号精确查询学生信息的函数
    char id[30];                             // 定义字符数组id，用于存储用户输入的要查询的学号
    printf("请输入要查询的学号: ");          // 提示用户输入要查询的学号
	scanf("%s", id);                         // 读取用户输入的学号，存入id数组
    Student *p = head;                       // 定义指针p，指向链表的头节点head，开始遍历链表
    while(p) {                               // 循环遍历链表：只要p不为空（还没到链表末尾），就继续
        if(strcmp(p->num, id) == 0) {                                                                                // 比较当前节点p的学号num和输入的id，strcmp相等返回0
            printf("\n查询结果:\n%-20s%-15s%-8s%-8s%-8s%-8s\n", "学号","姓名","语文","数学","英语","总分");  // 找到匹配学号，打印查询结果的表头
            printf("%-20s%-15s%-8.1f%-8.1f%-8.1f%-8.1f\n", p->num, p->name, p->chinese, p->math, p->english, p->sum);// 打印找到的学生信息，按指定格式输出各字段
            return;                          // 找到后直接返回，退出当前函数
        }
        p = p->next;                         // 没找到匹配，p指向下一个节点，继续遍历
    }
    printf("未找到学号为 %s 的记录。\n", id);// 遍历完整个链表都没找到，提示未找到该学号的记录
}

/*
 * 函数名称：search_fuzzy
 * 功能描述：
 *      实现按姓名模糊查询学生信息的功能。接收用户输入的姓名关键字，遍历存储学生信息的链表，
 *      通过strstr函数在学生姓名中查找关键字（模糊匹配），若匹配成功则按指定格式打印该学生信息，
 *      同时统计匹配到的记录总数；遍历完成后，若未匹配到任何记录则提示无结果，否则输出匹配到的记录数。
 * 输入参数：无
 * 返回值：void - 无返回值
 * 作者：陈浩桐 
 * 学号：20245080905093
 */
void search_fuzzy() {             									  // 按姓名模糊查询学生信息的函数
    char key[15]; int count = 0;  									  // 定义字符数组key存储查询关键字，count记录匹配到的学生数量（初始化为0）
    printf("请输入姓名关键字: ");  									 // 提示用户输入姓名关键字
	scanf("%s", key);               // 读取用户输入的关键字，存入key数组
    printf("\n模糊匹配结果:\n");    // 打印模糊匹配结果的提示
    Student *p = head;              // 定义指针p，指向链表头节点head，开始遍历
    while(p) {                      // 循环遍历链表：只要p不为空，就继续
        if(strstr(p->name, key)) {  // strstr函数：在p->name中查找key，找到返回非空指针（模糊匹配成功)
            printf(FORMAT, DATA);   // 打印匹配到的学生信息（FORMAT是自定义格式宏，DATA是学生信息字段宏）
            count++;                // 匹配到一条，计数加1
        }
        p = p->next;                // p指向下一个节点，继续遍历
    }
    if(count == 0) printf("未找到包含 \"%s\" 的学生。\n", key);         // 如果count为0，说明没有匹配到任何学生
    else printf("共找到 %d 条记录。\n", count);                         // 否则，打印找到的记录总数
}


// ================= 修改与删除 =================

/*
 *函数名称：modify
 *功能描述： 基于用户输入的学号查找学生记录，提供交互式界面更新姓名与各科成绩，
 *      并自动同步计算总分与修改时间戳。修改完成后立即触发本地二进制数据的存盘操作，
 *      确保内存与文件数据的一致性。若未找到对应学号，则向用户反馈提示信息。
 *输入参数：无
 *返回值：void
 *作者：黄子淳
 *学号：20245080905051 
*/
void modify() {																// 修改学生信息函数
    char id[30];															// 定义字符数组用于存储要查找的学号
    printf("\n请输入要修改的学生学号: "); scanf("%s", id);					// 提示用户输入要修改的学生学号，读取用户输入的学号 
    Student *p = head;														// 从头节点开始遍历链表
    while(p) {																// 遍历整个链表查找目标学生
        if(strcmp(p->num, id) == 0) {										// 比较当前节点学号与目标学号是否相同
            printf("找到学生: %s，请输入新信息:\n", p->name);				// 找到学生，提示输入新信息
            printf("新姓名: "); scanf("%s", p->name);						// 修改学生姓名
            printf("新语文: "); scanf("%lf", &p->chinese);					// 修改语文成绩
            printf("新数学: "); scanf("%lf", &p->math);						// 修改数学成绩
            printf("新英语: "); scanf("%lf", &p->english);					// 修改英语成绩
            p->sum = p->chinese + p->math + p->english;						// 重新计算总分（语文+数学+英语）
            get_current_time(p->timestr, &p->timeStamp);    				// 更新修改时间，获取当前系统时间
            printf("修改成功！\n");											// 显示修改成功提示
            saveData();														// 将修改后的数据保存到二进制文件
            wait_for_enter();												//等待用户按回车继续 
            return;															//修改完成退出函数 
        }
        p = p->next;														// 移动到链表中的下一个节点
    }
    printf("未找到该学生。\n");												// 遍历完整个链表仍未找到目标学号
    wait_for_enter();														// 等待用户按回车键继续
}

/*
 * 函数名称：delete_stu
 * 功能描述：
 *		删除指定学生信息功能。通过用户输入的学号，在全局链表中查找匹配的学生节点。
 *		如果找到目标节点，会提示用户确认删除操作，防止误删。确认删除后，根据节点位置
 *		（头节点、中间节点、尾节点）采用不同的链表调整策略，安全移除目标节点并释放内存。
 *		删除成功后自动保存数据到文件，确保数据一致性。若未找到目标学号，则给出相应提示。
 * 输入参数：无
 * 返回值：void
 * 作者：罗桂松 
 * 学号：20245080905055
 */
void delete_stu() {
    char id[30];															// 存储要删除的学号
    printf("\n请输入要删除的学号: "); scanf("%s", id);
    Student *p = head, *pre = NULL;											// p: 当前正在检查的节点（游标），pre: p的前一个节点（跟随者）。注意：pre初始为NULL，因为头节点没有前一个节点 
    // 遍历链表查找要删除的学号
    while(p) {																// 当p不是NULL时继续循环
        if(strcmp(p->num, id) == 0) {										// 找到匹配的学号
            printf("确定删除 %s 吗？(y/n): ", p->name);
            char cf[2]; scanf("%s", cf);
            if(strcmp(cf,"y")==0 || strcmp(cf,"Y")==0) { 					// 执行删除操作
                if(!pre) head = p->next; 									// 如果要删除的是头节点,头指针指向下一个节点
                else pre->next = p->next;									// 前一个节点跳过当前节点

                free(p);													// 释放内存
                printf("删除成功！\n");
                saveData(); 												// 保存修改
            }
            wait_for_enter();
            return;
        }
        pre = p; p = p->next;
    }
    printf("未找到该学号。\n");
    wait_for_enter();
}

// ================= 排序子模块 (增加时间排序) =================

/* 函数名称：sort
* 功能描述：
*     提供学生记录的排序功能子菜单。首先清屏并显示排序选项，根据用户选择
*     调用相应的排序函数（按总分降序或按录入时间最新排列），排序完成后
*     自动调用显示函数（display）展示排序结果。若用户选择返回主菜单，
*     则直接退出本函数。
* 输入参数：无
* 返回值：void - 无返回值
* 作者：王智斌 
*学号：20245080905073
*/
void sort() {                                                    // 定义排序函数，无返回值
    int sel;                                                     // 声明整型变量sel，用于存储用户选择
    system("cls");                                               // 清屏，清除控制台之前的内容
    printf("\n======= 排序子菜单 =======\n");                    // 打印排序子菜单标题
    printf("1. 按总分降序排列\n");                               // 打印选项1：按总分降序排列
    printf("2. 按录入时间最新排列\n");                           // 打印选项2：按录入时间最新排列
    printf("0. 返回主菜单\n");                                   // 打印选项0：返回主菜单
    printf("==========================\n请选择: ");              // 打印分隔线和提示语
    scanf("%d", &sel);                                           // 读取用户输入的整数并存入sel变量
    if(sel == 1) sort_by_sum();                                  // 如果用户输入1，调用按总分排序函数
    else if(sel == 2) sort_by_time();                            // 如果用户输入2，调用按时间排序函数
    else return;                                                 // 如果输入其他值（包括0），直接返回上级菜单
    display();                                                   // 调用显示函数，展示排序后的结果
}

/*
 * 函数名称：sort_by_sum
 * 功能描述：
 *     对学生记录链表按总分进行降序排序。如果链表为空则直接返回。
 *     使用双重循环遍历链表，比较相邻节点的总分，若当前节点总分小于后续节点，
 *     则调用swap_data函数交换两节点的数据。排序完成后输出提示信息。
 * 输入参数：无
 * 返回值：void - 无返回值
 * 作者：王智斌 
 * 学号：20245080905073
 */
void sort_by_sum() {                                                    // 定义按总分排序函数
    if(!head) return;                                                   // 如果链表为空，直接返回
    Student *p, *q;                                                     // 定义两个指针变量，用于遍历链表
    for(p=head; p; p=p->next)                                           // 外层循环：从头节点开始遍历每个节点
        for(q=p->next; q; q=q->next)                                    // 内层循环：从p的下一个节点开始遍历
            if(p->sum < q->sum) swap_data(p, q);                        // 如果p的总分小于q，交换两者数据
    printf("总分降序排序完成！\n");                                     // 输出排序完成提示
}

/*
 * 函数名称：sort_by_time
 * 功能描述：
 *     对学生记录链表按录入时间进行降序排序（最新记录在前）。
 *     如果链表为空则直接返回。使用双重循环遍历链表，比较相邻节点的时间戳，
 *     若当前节点时间戳小于后续节点，则调用swap_data函数交换两节点的数据。
 *     排序完成后输出提示信息。
 * 输入参数：无
 * 返回值：void - 无返回值
 * 作者：王智斌 
 * 学号：20245080905073
 */
void sort_by_time() {                                                   // 定义按时间排序函数
    if(!head) return;                                                   // 如果链表为空，直接返回
    Student *p, *q;                                                     // 定义两个指针变量，用于遍历链表
    for(p=head; p; p=p->next)                                           // 外层循环：从头节点开始遍历每个节点
        for(q=p->next; q; q=q->next)                                    // 内层循环：从p的下一个节点开始遍历
            if(p->timeStamp < q->timeStamp)                             // 如果p的时间戳小于q的时间戳（p比q旧）
                swap_data(p, q);                                        // 交换p和q的数据，使较新的记录在前
    printf("按时间最新排序完成！\n");                                   // 输出排序完成提示信息
}

// ================= 统计与辅助 =================

/*
 *函数名称：statistics
 * 功能描述：
 *        对当前系统中的学生数据进行综合统计，计算并展示核心统计指标。清屏后，
 *        若数据为空则提示用户，否则遍历链表获取学生人数、总分平均值、最高分及对应学生、
 *        最低分及对应学生，并格式化输出简明统计报告，为数据分析和教学评估提供直观参考。
 *输入参数：无
 *返回值：void
 *作者：黄子淳
 *学号：20245080905051 
*/
void statistics() {																	// 统计函数：计算学生数据的基本统计信息
    system("cls");																	// 清屏，为显示统计结果准备干净的界面（system为window的清屏命令） 
    if(!head) { printf("没有数据可统计。\n"); wait_for_enter(); return; }			// 检查链表是否为空（没有学生数据），提示用户当前没有可统计的数据，等待用户按回车键继续，结束函数执行 
    int count = 0; double total = 0, max_s = -1, min_s = 999;						// 初始化统计变量：count记录学生人数，total累计总分，max_s记录最高分（初始-1），min_s记录最低分（初始999） 
    Student *p = head, *max_p = head, *min_p = head;								// 定义遍历指针和记录指针：p用于遍历链表，max_p指向最高分学生，min_p指向最低分学生，均初始化为头节点
    while(p) {																		// 遍历链表中的所有学生节点
        count++;																	// 统计人数：每遍历一个节点，计数器加1
        total += p->sum;															// 累加总分：将当前学生的总分加到total中
        if(p->sum > max_s) { max_s = p->sum; max_p = p; }							// 更新最高分记录：如果当前学生总分大于已知最高分，更新最高分数值和指向最高分学生的指针
        if(p->sum < min_s) { min_s = p->sum; min_p = p; }							// 更新最低分记录：如果当前学生总分小于已知最低分，更新最低分数值和指向最低分学生的指针
        p = p->next;																// 移动到链表中的下一个节点
    }
    printf("\n---------- 统计分析报告 ----------\n");								// 输出统计结果的标题分隔线
    printf("总人数:   %d 人\n", count);												// 输出总人数统计结果
    printf("平均总分: %.2lf 分\n", total / count);									// 输出平均总分：总分除以人数，保留两位小数
    printf("最高分:   %.2lf (%s)\n", max_s, max_p->name);							// 输出最高分信息：分数和对应的学生姓名
    printf("最低分:   %.2lf (%s)\n", min_s, min_p->name);							// 输出最低分信息：分数和对应的学生姓名
    printf("----------------------------------\n");									// 输出统计结果的结束分隔线
    wait_for_enter();																// 等待用户查看完统计结果后按回车键继续
}

/*
* 函数名称：swap_data
* 功能描述：
*   安全交换两个学生节点除链表指针外的全部数据内容。通过计算结构体偏移量
*   精确复制除next指针外的所有数据字段，实现节点数据的原位交换，避免链表
*   结构的破坏。此方法在排序算法中用于调整学生顺序，同时保持链表原有的连
*   接关系。
* 输入参数：
*   Student *a - 指向第一个学生节点的指针
*   Student *b - 指向第二个学生节点的指针
* 返回值：void
* 作者：黄子淳 
* 学号：20245080905054
*/
void swap_data(Student *a, Student *b) {											// 函数：交换两个学生节点的数据（不交换next指针）
    Student temp;																	// 声明临时变量用于存储交换过程中的数据
    // 交换除next指针外的所有数据字段
    memcpy(&temp, a, sizeof(Student) - sizeof(Student*));							// 将节点a的数据（除next指针外）复制到临时变量temp中， sizeof(Student) - sizeof(Student*) 计算除指针外的数据大小
    memcpy(a, b, sizeof(Student) - sizeof(Student*));								// 将节点b的数据（除next指针外）复制到节点a的对应位置， 使用相同的大小计算，确保不覆盖next指针
    memcpy(b, &temp, sizeof(Student) - sizeof(Student*));							// 将临时变量temp中的数据（原a节点数据）复制到节点b，完成两个节点数据的交换
}

/*
 *函数名称： get_current_time
 *功能描述：
 *       获取系统当前时间并生成双重格式的时间信息。通过调用标准时间库函数获取
 *       原始时间戳，将其转换为本地时间结构体，并同时生成易读的字符串格式时间
 *       与可计算的整数时间戳。确保时间记录的一致性与格式标准化，为数据操作提
 *       供准确的时间维度信息。
 *输入参数：
 *  char *tstr  - 指向字符数组的指针，用于接收格式化的时间字符串
 *  long *tstamp - 指向长整型变量的指针，用于接收原始时间戳
 *返回值：void
 *作者：黄子淳
 *学号：20245080905051 
*/
void get_current_time(char *tstr, long *tstamp) {								// 函数：获取当前时间并格式化为字符串和时间戳
    time_t t; struct tm *lt; time(&t); lt = localtime(&t);						// 声明时间变量和时间结构体指针，获取当前系统时间（从1970年1月1日至今的秒数），将time_t类型的时间转换为本地时间结构体
    *tstamp = (long)t; strftime(tstr, 35, "%Y-%m-%d %H:%M:%S", lt);				// 将时间戳（秒数）存储到第二个参数指向的内存位置
	                                                                            // 将时间结构体格式化为易读的字符串，存储到第一个参数指向的内存位置        
																				// 格式为：年-月-日 时:分:秒（例如：2024-01-15 14:30:25）
                                                                                // 35是缓冲区大小，确保能容纳格式化后的字符串
}

/* 
 *函数名称：insert
 *功能描述：
 *        实现在指定学生学号之后插入新的学生信息。若当前系统无学生数据或目标学号不存在，
 		  则提示用户并退出；否则，读取新学生信息并完成学号唯一性校验，计算总分与时间戳，
		  最后通过链表指针操作完成插入。所有操作完成后自动保存数据至二进制文件，确保系统状态一致。
 *输入参数值：无
 *返回值：void
 *作者：黄子淳
 *学号：20245080905051 
 */
void insert() {
    if (head == NULL) {																			//检查链表是否为空
        printf("\n[提示] 当前系统中没有学生数据，无法在指定位置插入，请先使用录入功能。\n");	//若链表为空输出 [提示] 当前系统中没有学生数据，无法在指定位置插入，请先使用录入功能。
        wait_for_enter();																		// 等待用户按回车继续
        return;																					//结束函数执行 
    }

    char targetID[30];																			// 用于存储目标学号的字符数组
    printf("\n请输入要在哪个学号之后插入新信息: ");												// 读取用户输入的目标学号
    scanf("%s", targetID);																		//输入目标学号 

    // 1. 寻找目标位置节点
    Student *p = head;																			// 从头节点开始遍历
    while (p != NULL && strcmp(p->num, targetID) != 0) {										// 遍历链表，查找匹配的学号
        p = p->next;																			// 移动到下一个节点
    }

    // 如果遍历完都没找到
    if (p == NULL) {
        printf("[错误] 未找到学号为 %s 的学生，无法完成插入。\n", targetID);					//输出 [错误] 未找到学号为 %s 的学生，无法完成插入。
        wait_for_enter();																		//等待用户按回车继续 
        return;																					//结束函数执行 
    }

    // 2. 准备新学生数据
    Student *newNode = (Student *)malloc(sizeof(Student));										// 为新学生分配内存空间
    printf("\n已找到目标位置。请输入新学生的信息：\n");											//输出 已找到目标位置。请输入新学生的信息：
    
    printf("新学号: "); 																		//输出新学号： 
    scanf("%s", newNode->num);																	//输入新学号为%s 
    
    // 插入前也需要查重：防止新插入的学号与现有学号冲突
    Student *check = head;																		// 从头节点开始检查
    while(check) {					
        if(strcmp(check->num, newNode->num) == 0) {												//如果发现重复学号
            printf("[错误] 该学号 %s 在系统中已存在，插入取消！\n", newNode->num);				//输出 [错误] 该学号 %s 在系统中已存在，插入取消！
            free(newNode);																		// 释放已分配的内存
            wait_for_enter();																	//等待用户按回车继续 
            return;																				//结束函数执行 
        }
        check = check->next;																	//移动到下一个节点
    }

    printf("姓名: "); scanf("%s", newNode->name);												//输出姓名：，在键盘中输入%s的姓名 
    printf("语文成绩: "); scanf("%lf", &newNode->chinese);										//输出语文成绩：，在键盘中输入%lf的语文成绩 
    printf("数学成绩: "); scanf("%lf", &newNode->math);											//输出数学成绩：，在键盘中输入%lf的数学成绩 
    printf("英语成绩: "); scanf("%lf", &newNode->english);										//输出英语成绩：，在键盘中输入%lf的英语成绩 
    
    // 计算总分并获取当前时间戳
    newNode->sum = newNode->chinese + newNode->math + newNode->english;							// 计算总分
    get_current_time(newNode->timestr, &newNode->timeStamp);									// 获取当前时间戳并存储在节点中（get_current_time为自定义获取时间函数） 
																								//（newNode->timestr） 存储格式化的时间文本（&newNode->timeStamp） 时间戳变量的地址
    // 3. 执行链表指针操作 (核心逻辑)
    // 假设顺序是：p -> (原本p的后继)
    // 变为：p -> newNode -> (原本p的后继)
    newNode->next = p->next;																	// 将新节点的next指向原目标节点的下一个节点
    p->next = newNode;																			// 将原目标节点的next指向新节点

    printf("\n[成功] 已在学号 %s 之后成功插入新学生 %s。\n", targetID, newNode->name);			//输出 [成功] 已在学号 %s 之后成功插入新学生 %s。
    
    // 4. 保存更改到二进制文件
    saveData(); 																				// 调用保存函数将数据写入文件
    wait_for_enter();																			//等待用户按回车继续
}


/* ================= CSV 数据交换模块 ================= */

/*
 * 函数名称：exportToCSV
 * 功能描述：
 * 		导出模块。将链表数据保存为 CSV 格式文件，以便于在 Excel 等办公软件中直接查看。
 * 		函数程序中指定分隔符(sep=,)以及采用公式化文本输出(=" ")的技巧，
 * 		有效解决了 CSV 文件在不同环境下的分列兼容性及长学号科学计数法显示的问题。
 * 输入参数：无
 * 返回值：void
 * 作者：郑永炫
 */
void exportToCSV() {
    FILE *fp = fopen("students.csv", "w");							// 以只写模式("w")打开文件，若文件已存在则会被覆盖
    if (fp == NULL) {
        printf("导出失败，文件可能被 Excel 占用。\n");
        return;
    }
	
    fprintf(fp, "sep=,\n"); 										// 不同电脑的设置有差异，让 Excel自动识别逗号分隔符
    fprintf(fp, "学号,姓名,语文,数学,英语,总分,录入时间\n");		// 创建表格标题栏 
    
    Student *p = head;																				// 创建临时指针p，从头开始遍历 
    while (p) {																						// 遍历到末尾 NULL 
        fprintf(fp, "=\"%s\",%s,%.1f,%.1f,%.1f,%.1f,%s\n",											// 特色设计，将学号字段改为 ="%s" 的格式，Excel会把它当成公式，显示的就是原始文本 
                p->num, p->name, p->chinese, p->math, p->english, p->sum, p->timestr);				// 避免了长数字在 Excel是科学计数法的问题 
        p = p->next;																				// 进入下一个学生节点 
    }
    fclose(fp);																						// 保存并关闭文本工作 
    printf("\n[成功] CSV 文件已导出。请用 Excel 打开以查看。\n");
}

//  从 CSV 导入 (从 Excel 编辑后导回程序)
void importFromCSV() {
    FILE *fp = fopen("students.csv", "r");											// 只读模式打开 CSV文件
    if (fp == NULL) {
        printf("未找到 students.csv 文件。\n");
        return;
    }

    printf("导入将覆盖当前系统数据，确定吗？(y/n): ");
    char cf[2]; scanf("%s", cf);													// 使用临时获取键盘输入的值 
    if(strcmp(cf, "y") != 0 && strcmp(cf, "Y") != 0) { fclose(fp); return; }		// 如果输入的值是 y 或 Y，不会进入 if语句，反之进入 if语句保存并关闭文件流工作 

    while(head) { Student *tmp = head; head = head->next; free(tmp); }				// 清空当前链表，销毁所有学生节点，为后续工作准备空间 

    char line[512];											// 定义一行缓冲区，用于逐行读取 CSV
    while (fgets(line, sizeof(line), fp)) {					// 技巧：循环读取，直到读到包含“学号”这两个字的表头行
        if (strstr(line, "学号")) break;					// 从而跳过整个标题栏，标题栏的下一行开始就是学生信息 
    }

    int count = 0;											// 用于统计成功导入的学生数量
    while (fgets(line, sizeof(line), fp)) {					// 从表头下一行开始，逐行读取学生记录
        if (strlen(line) < 5) continue; 					// 如果这一行短到不可能是一条合法的学生记录，直接跳过

        Student *n = (Student *)malloc(sizeof(Student));	// 为每一条即将导入的学生记录在堆上分配独立内存，作为链表节点
        
        /* 关键代码解析：
           %*[^\"] 意思是：跳过前面的所有字符直到遇到引号
           \"%[^\"]\" 意思是：读取引号内的内容存入 n->num
           接下来是逗号，然后读取下一段到逗号为止的内容存入 n->name
        */
        if (sscanf(line, "%*[^\"]\"%[^\"]\",%[^,],%lf,%lf,%lf,%lf,%[^,\n]",	n->num, n->name, &n->chinese, &n->math, &n->english, &n->sum, n->timestr) >= 6) {
            
            // 自动补齐缺失的时间戳
            n->timeStamp = (long)time(NULL); 	// 如果 CSV 中时间缺失，自动补齐缺失的时间戳
            n->next = head;						// 头插法将新学生加入链表
            head = n;
            count++;							// 导入成功计数
        } else {
            // 如果不是 =" " 格式，尝试普通格式读取（兼容直接手写的情况）
            if (sscanf(line, "%[^,],%[^,],%lf,%lf,%lf,%lf", n->num, n->name, &n->chinese, &n->math, &n->english, &n->sum) >= 6) {
                get_current_time(n->timestr, &n->timeStamp);	// 自动生成时间字符串和时间戳
                n->next = head;									// 同样采用头插法加入链表
                head = n;
                count++;										// 导入成功计数
            } else {
                free(n);										// 两种格式都解析失败，释放之前申请的内存空间 
            }
        }
    }

    fclose(fp);													// 保存并关闭文件流 
    saveData(); 												// 同步回二进制文件
    printf("\n[成功] 已从 CSV 成功导入 %d 条记录！\n", count);
}
