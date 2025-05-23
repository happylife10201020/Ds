/**다항식 덧셈 프로그램
 * 충북대학교 소프트웨어학부 2022041069 이인수
 * class : 자료구조
 * pf : 이의종
 *
 * txt파일을 불러와 다항식으로 표현하고
 * 초기배열방식
 * 개선된 배열 방식
 * 연결리스트 방식
 * 으로 계산한 후 계산 결과값과 소요시간을 출력하는 프로그램
 */


/////////////////////////////////////////////////////DECLARE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SWAP(x, y) {int temp = x; x = y; y = temp;}
#define IsZero(x) (!(x)? 1 : 0)
#define COMPARE(x, y) (x > y ? 1 : x < y ? -1 : 0) // x가 y보다 크면 1 같으면 0 작으면 -1

#define BASIC_REPEAT 10 // 초기 배열 방식 반복: ms단위 정밀측정을을 위해 ,큰 차수에서 실행 시간 과다를 방지하기 위해 10번 반복
#define OTHER_REPEAT 1000000 // 개선된 배열/연결 리스트: 시간 측정 정밀도를 위해 100만 번 반복
#define PRINT_ALL 1 // 0 일때는 다항식 덧셈 결과 하나만 출력 1일때 3개 출력`



//Declare Funtions (초기배열 방식, 개선된 방식, 연결리스트 방식, 정렬)
double addPolyArrayBasic(FILE *, int **, int **, int, int);
double addPolyArrayOptimized(FILE *,int **, int **, int, int);
double addPolyArrayLinkedList(FILE *,int **, int **, int, int);
void sortArray(int **, int);


typedef struct Node {
    int coef;
    int exp;
    struct Node *next;
}NODE;

NODE *LinkedListAdd(NODE *Head1, NODE *Head2);
void attach(NODE ** rear_tail, int coef, int exp);



/////////////////////////////////////////////////////CODE

int main(void) {
    /** open input file and sort.**/
    FILE *fp = fopen("input.txt", "r");
    if (fp == NULL) {
        fprintf(stderr, "Error opening file input.txt\n");
        exit(EXIT_FAILURE);
    }

    FILE *fp2 = fopen("output.txt", "w");
    if (fp2 == NULL) {
        fprintf(stderr, "Error opening file output.txt\n");
        exit(EXIT_FAILURE);
    }

    int numberOfExponent1, numberOfExponent2;
    fscanf(fp, "%d", &numberOfExponent1);
    fscanf(fp, "%d", &numberOfExponent2);


    int **data1, **data2;
    if (numberOfExponent1 == 0) {
        data1 = malloc(sizeof(int *));
        if (data1 == NULL) {
            fprintf(stderr, "Error allocating memory for data1\n");
            exit(1);
        }
        for (int i = 0; i<2; i++) {
            data1[i] = calloc(2, sizeof(int));
        }
    }else {
        data1 = malloc(numberOfExponent1 * sizeof(int*));
        for (int i = 0; i < numberOfExponent1; i++) {
            data1[i] = malloc(2 * sizeof(int));
            fscanf(fp, "%d", &data1[i][0]);
            fscanf(fp, "%d", &data1[i][1]);
        }
    }

    if (numberOfExponent2 == 0) {
        data2 = malloc(sizeof(int *));
        if (data2 == NULL) {
            fprintf(stderr, "Error allocating memory for data2\n");
            exit(1);
        }
        for (int i = 0; i<2; i++) {
            data2[i] = calloc(2, sizeof(int));
        }
    }else {
        data2 = malloc(numberOfExponent2 * sizeof(int*));
        for (int i = 0; i < numberOfExponent2; i++) {
            data2[i] = malloc(2 * sizeof(int));
            fscanf(fp, "%d", &data2[i][0]);
            fscanf(fp, "%d", &data2[i][1]);
        }
    }

    //SORT algorithm
    sortArray(data1, numberOfExponent1);
    sortArray(data2, numberOfExponent2);


    //print array
    int isFirst = 1;
    for (int i = 0; i<numberOfExponent1; i++) {
        const int coef = data1[i][0];
        const int exp = data1[i][1];

        if (coef == 0)continue;

        if (i > 0 && coef > 0) fprintf(fp2, "+");

        if (exp != 0) {
            fprintf(fp2, "%dx^%d", data1[i][0], data1[i][1]);
        }else {
            fprintf(fp2, "%d", data1[i][0]);
        }
        isFirst = 0;
    }
    if (isFirst) fprintf(fp2, "0");
    fprintf(fp2, "\n");

    isFirst = 1;
    for (int i = 0; i<numberOfExponent2; i++) {
        const int coef = data2[i][0];
        const int exp = data2[i][1];

        if (coef == 0)continue;

        if (i > 0 && coef > 0) fprintf(fp2, "+");

        if (exp != 0) {
            fprintf(fp2, "%dx^%d", data2[i][0], data2[i][1]);
        }else {
            fprintf(fp2, "%d", data2[i][0]);
        }
        isFirst = 0;
    }
    if (isFirst) fprintf(fp2, "0");
    fprintf(fp2, "\n");


    //do add poly and get runningtime
    double basicTimeSpend = addPolyArrayBasic(fp2, data1, data2, numberOfExponent1, numberOfExponent2);
    double optimizedTimeSpend = addPolyArrayOptimized(fp2, data1, data2, numberOfExponent1, numberOfExponent2);
    double LinkedListTimeSpend = addPolyArrayLinkedList(fp2, data1, data2, numberOfExponent1, numberOfExponent2);


    //print running time
    fprintf(fp2, "%f\t%f\t%f", basicTimeSpend, optimizedTimeSpend, LinkedListTimeSpend);

    //free
    if (numberOfExponent1 == 0) {
        free(data1[0]);
    }else {
        for (int i = 0; i < numberOfExponent1; i++) free(data1[i]);
    }
    if (numberOfExponent2 == 0) {
        free(data2[0]);
    }else {
        for (int i = 0; i < numberOfExponent2; i++) free(data2[i]);
    }
    free(data1);
    free(data2);

    fclose(fp);
    fclose(fp2);
    return 0;
}

/** 2차원배열 정렬알고리즘
    * 선택정렬
    * 시간복잡도 : O(N^2)
    * 매개변수:
    *   **arr : 2차원 배열
    *   n : 항의 개수
    *
    *  계수가 0이고 지수가 0이 아닌 항이 있다면 축약
    *  계수가 같은 항이 있다면 축약*/
void sortArray(int **arr, int len) {

    for (int i = 0; i < len; i++) {
        if (arr[i][0] == 0 && arr[i][1] != 0) arr[i][1] = 0;
    }

    for (int i = 0; i < len-1; i++) {
        for (int j = i+1; j < len; j++) {
            if (arr[i][1] == arr[j][1]) {
                arr[i][0] += arr[j][0];
                arr[j][0] = 0;
                arr[j][1] = 0;
            }
            if (arr[i][1] < arr[j][1]) {
                SWAP(arr[i][0], arr[j][0]);
                SWAP(arr[i][1], arr[j][1]);
            }
        }
    }
}

/** 초기 배열 방식
     * e.g)
     *  A(x) = x^4 + 10x^3 + 3x^2 + 1
     *  A = (4, 1, 10, 3, 0, 1)
     *
     *  첫번째 value = 최고차항의 지수
     *  그 이후는 최고차항부터 내림차순으로의 지수
     *
     * 매개변수
     * data1, data2 : 각 다항식의 지수와 계수
     * n, m : 정렬된 항의 수 */
double addPolyArrayBasic(FILE * output,int **data1, int **data2, int n, int m) {
    clock_t begin = clock();

    // 최고 차수와 최저 차수 계산
    int maxd1 = n > 0 ? data1[0][1] : 0; // 최고 차수 (내림차순 정렬이므로 첫 번째)
    int mind1 = n > 0 ? data1[n-1][1] : 0; // 최저 차수 (마지막 항)
    int maxd2 = m > 0 ? data2[0][1] : 0;
    int mind2 = m > 0 ? data2[m-1][1] : 0;

    // 전체 다항식의 차수 범위
    int max_degree = maxd1 > maxd2 ? maxd1 : maxd2;
    int min_degree = mind1 < mind2 ? mind1 : mind2;
    int range = max_degree - min_degree + 1; // 배열 크기 (최고차수 ~ 최저차수)

    // 배열 할당
    int *poly1 = calloc(range, sizeof(int));
    if (poly1 == NULL) {
        fprintf(stderr, "Error allocating memory for poly1\n");
        exit(1);
    }
    int *poly2 = calloc(range, sizeof(int));
    if (poly2 == NULL) {
        fprintf(stderr, "Error allocating memory for poly2\n");
        free(poly1);
        exit(1);
    }
    int *sumOfPoly = calloc(range, sizeof(int));
    if (sumOfPoly == NULL) {
        fprintf(stderr, "Error allocating memory for sumOfPoly\n");
        free(poly1);
        free(poly2);
        exit(1);
    }

    // 다항식 데이터를 배열에 매핑
    for (int i = 0; i < n; i++) {
        if (data1[i][0] != 0) {
            int index = max_degree - data1[i][1]; // max_degree를 기준으로 인덱스 계산
            poly1[index] = data1[i][0];
        }
    }
    for (int i = 0; i < m; i++) {
        if (data2[i][0] != 0) {
            int index = max_degree - data2[i][1];
            poly2[index] = data2[i][0];
        }
    }

    // 덧셈 수행 정밀한 시간측정을 위해 BASIC_REPEAT번 반복
    for (int i = 0; i < BASIC_REPEAT; i++) {
        for (int j = 0; j < range; j++) {
            sumOfPoly[j] = poly1[j] + poly2[j];
        }
    }

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC * 1000.0;

    // 출력
    if (PRINT_ALL) {
        int count = 0;
        for (int i = 0; i < range; i++) {
            if (sumOfPoly[i] != 0) {
                if (sumOfPoly[i] > 0 && count > 0) fprintf(output, "+");
                int exp = max_degree - i; // 지수 계산
                if (exp != 0) {
                    fprintf(output, "%dx^%d", sumOfPoly[i], exp);
                } else {
                    fprintf(output, "%d", sumOfPoly[i]);
                }
                count++;
            }
        }
        if (count == 0) fprintf(output, "0");
        fprintf(output, "\n");
    }

    // 메모리 해제
    free(poly1);
    free(poly2);
    free(sumOfPoly);

    return time_spent / BASIC_REPEAT;
}

/**개선된 배열 방식 */
double addPolyArrayOptimized(FILE * output, int ** data1,  int ** data2, /**첫번째 다항식 항의 수*/int n, /**두번째 다항식 항의 수*/int m) {
    /* 개선된 배열 방식
     *
     * 매개변수
     * data1, data2 : 각 다항식의 지수와 계수
     * n, m : 정렬된 항의 수
     *
     * **/

    clock_t begin = clock();

    /*두 다항식 배열을 붙여논 것*/
    int **sumArray;
    /*두 다항식에 덧셈연산을 수행한 2차원배열*/
    int **resultArray;
    int resultCount; //두 다항식의 합의 index를 가르키고 있는 값

    // 개선된 배열/연결 리스트: 시간 측정 정밀도를 위해 100만 번 반복
    for (int i = 0; i<OTHER_REPEAT; i++) {
        //두 다항식을 하나의 다항식으로 표현
        if (i != 0) {
            for (int j = 0; j < n+m; j++) {
                free(sumArray[j]);
                free(resultArray[j]);
            }
            free(sumArray);
            free(resultArray);
        }

        sumArray = malloc((n+m)*sizeof(int*));
        for (int i = 0; i < n+m; i++) {
            sumArray[i] = malloc(2*sizeof(int));
            if (i<n) {
                sumArray[i][0] = data1[i][0];
                sumArray[i][1] = data1[i][1];
            }else {
                sumArray[i][0] = data2[i-n][0];
                sumArray[i][1] = data2[i-n][1];
            }
        }

        resultArray = calloc((n+m), sizeof(int*));
        for (int i = 0; i < n+m; i++) {
            resultArray[i] = calloc(2,sizeof(int));
        }
        resultCount = 0;

        int count1 = 0; //개선된 형태의 배열에서 첫 번째 다항식의 index를 가르키고 있는 값
        int count2 = n; //개선된 형태의 배열에서 두 번째 다항식의 index를 가르키고 있는 값
        while (1) {
            if (count1 >= n && count2 >= n + m) break;
            if (count1 >= n) {
                resultArray[resultCount][0] = sumArray[count2][0];
                resultArray[resultCount][1] = sumArray[count2][1];
                count2++; resultCount++;
                continue;
            }
            if (count2 >= n + m) {
                resultArray[resultCount][0] = sumArray[count1][0];
                resultArray[resultCount][1] = sumArray[count1][1];
                count1++; resultCount++;
                continue;
            }
            switch (COMPARE(sumArray[count1][1], sumArray[count2][1])) {
                case -1: {
                    resultArray[resultCount][0] = sumArray[count2][0];
                    resultArray[resultCount][1] = sumArray[count2][1];
                    count2++;
                    resultCount++;
                } break;
                case 0: {
                    resultArray[resultCount][0] = sumArray[count1][0] + sumArray[count2][0]; // 계수의 합
                    resultArray[resultCount][1] = sumArray[count1][1]; // 지수
                    count1++;
                    count2++;
                    resultCount++;
                }break;
                case 1:{
                    resultArray[resultCount][0] = sumArray[count1][0];
                    resultArray[resultCount][1] = sumArray[count1][1];
                    count1++;
                    resultCount++;
                } break;
            }
        }
    }
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC * 1000.0;

    //print
    if (PRINT_ALL) {
        int isFirst = 1;
        for (int i = 0; i<n+m; i++) {
            if (resultArray[i][0] == 0) continue;
            if (i > 0 && resultArray[i][0] > 0) fprintf(output, " + ");
            fprintf(output, "%d", resultArray[i][0]);
            if (resultArray[i][1] != 0) fprintf(output, "x^%d", resultArray[i][1]);
            isFirst = 0;
        }
        if (isFirst) fprintf(output, "0");
        fprintf(output, "\n");
    }



    //free
    for (int j = 0; j < n+m; j++) {
        free(sumArray[j]);
        free(resultArray[j]);
    }
    free(sumArray);
    free(resultArray);

    return time_spent/OTHER_REPEAT;
}


/**연결리스트 다항식 덧셈*/
double addPolyArrayLinkedList(FILE * output, int ** data1,  int ** data2, /**첫번째 다항식 항의 수*/int n, /**두번째 다항식 항의 수*/int m) {
    clock_t begin = clock();
    NODE *exp_Head1 = NULL;
    NODE *exp_Head2 = NULL;
    NODE *exp_Tail1 = NULL;
    NODE *exp_Tail2 = NULL;

    //두 개의 다항식을 연결리스트로 표현
    for (int i = 0; i<n;i++) {
        if (data1[i][0] == 0) continue;
        NODE *temp = malloc(sizeof(NODE));
        if (temp == NULL) {
            fprintf(stderr, "Memory allocation failed in addPolyArrayLinkedList\n");
            exit(1);
        }
        temp->coef = data1[i][0];
        temp->exp = data1[i][1];
        temp->next = NULL;
        if (exp_Head1 == NULL) {
            exp_Head1 = exp_Tail1 = temp;
        }else {
            exp_Tail1->next = temp;
            exp_Tail1 = temp;
        }
    }

    for (int i = 0; i<m; i++) {
        if (data2[i][0] == 0) continue;
        NODE *temp = malloc(sizeof(NODE));
        if (temp == NULL) {
            fprintf(stderr, "Memory allocation failed in addPolyArrayLinkedList\n");
            exit(1);
        }
        temp->coef = data2[i][0];
        temp->exp = data2[i][1];
        temp->next = NULL;
        if (exp_Head2 == NULL) {
            exp_Head2 = exp_Tail2 = temp;
        }else {
            exp_Tail2->next = temp;
            exp_Tail2 = temp;
        }
    }


    //두 다항식을 더한 값
    NODE *sum;
    // 개선된 배열/연결 리스트: 시간 측정 정밀도를 위해 100만 번 반복
    for (int i = 0; i<OTHER_REPEAT; i++) {
        sum = NULL;
        sum = LinkedListAdd(exp_Head1, exp_Head2);
    }
    if (sum == NULL) {
        sum = malloc(sizeof(NODE));
        if (sum == NULL) {
            fprintf(stderr, "Memory allocation failed in addPolyArrayLinkedList\n");
            exit(1);
        }
        sum->coef = 0;
        sum->exp = 0;
        sum->next = NULL;
    }
    clock_t end = clock();
    NODE *sum_check = sum;


    //print
    int fCount = 0;
    for (;sum_check != NULL; sum_check = sum_check->next) {
        if (sum_check->coef == 0) continue;

        if (sum_check->coef > 0) {
            if (fCount != 0) fprintf(output, "+");
        }

        fCount++;
        if (sum_check->exp != 0) {
            fprintf(output, "%dx^%d", sum_check->coef, sum_check->exp);
        }
        else fprintf(output, "%d", sum_check->coef);
    }
    if (fCount == 0) fprintf(output, "0");
    fprintf(output, "\n");


    //free
    while (exp_Head1 != NULL) {
        NODE *temp = exp_Head1;
        exp_Head1 = exp_Head1->next;
        free(temp);
    }
    while (exp_Head2 != NULL) {
        NODE *temp = exp_Head2;
        exp_Head2 = exp_Head2->next;
        free(temp);
    }
    while (sum != NULL) {
        NODE *temp = sum;
        sum = sum->next;
        free(temp);
    }

    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC * 1000.0;
    return time_spent/OTHER_REPEAT; //
}

/**연결리스트로 표현된 두 다항식 합쳐서 포인터 형태로 반환하는 함수*/
NODE *LinkedListAdd(NODE *Head1 /**다항식 1*/, NODE *Head2/**다항식 2*/) {

    NODE *rear = malloc(sizeof(NODE)); // 두 다항식을 더한 값 // 첫 번째 노드는 빈 값
    if (rear == NULL) {
        fprintf(stderr, "Memory allocation failed in LinkedListAdd\n");
        exit(1);
    }
    rear->next = NULL;
    NODE *rear_tail = rear;
    NODE *temp1 = Head1, *temp2 = Head2;
    while (temp1 != NULL && temp2 != NULL) {
        switch (COMPARE(temp1->exp, temp2->exp)) {
            case -1: {
                attach(&rear_tail, temp2->coef, temp2->exp);
                temp2 = temp2->next;
                break;
            }
            case 0: {
                attach(&rear_tail, temp1->coef + temp2->coef, temp1->exp);
                temp1 = temp1->next;
                temp2 = temp2->next;
                break;
            }
            case 1: {
                attach(&rear_tail, temp1->coef, temp1->exp);
                temp1 = temp1->next;
                break;
            }
        }
    }
    for (; temp1 != NULL; temp1 = temp1->next) attach(&rear_tail, temp1->coef, temp1->exp);
    for (; temp2 != NULL; temp2 = temp2->next) attach(&rear_tail, temp2->coef, temp2->exp);
    return rear->next; // 첫 번째 노드는 비어있는 값
}

/**연결리스트를 붙이는 함수*/
void attach(NODE ** rear_tail, int coef, int exp) {
    if (coef == 0) return; // 계수 0이면 스킵

    (*rear_tail)->next = malloc(sizeof(NODE));
    if ((*rear_tail)->next == NULL) {
        fprintf(stderr, "Memory allocation failed in attach\n");
        exit(1);
    }

    (*rear_tail) = (*rear_tail)->next;
    (*rear_tail)->coef = coef;
    (*rear_tail)->exp = exp;
    (*rear_tail)->next = NULL;
}