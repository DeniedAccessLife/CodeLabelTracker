//Принимает в качестве аргумента имя файла f с текстом программы с метками и печатает все метки, на которые нет ни одного перехода.

#include <stdio.h>
#include <stdlib.h>

#define MAX_LABELS 100
#define MAX_LINE_LENGTH 100

struct Label
{
    int label;
    int is_referenced;
};

// Функция для проверки, является ли символ пробельным
int isspace(int c)
{
    return (c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\v' || c == '\f');
}

// Функция для проверки, является ли символ цифрой
int isdigit(int c)
{
    return (c >= '0' && c <= '9');
}

// Собственная реализация функции для преобразования строки в число
int my_atoi(const char *str)
{
    int result = 0;

    while (isdigit(*str))
    {
        result = result * 10 + (*str - '0');
        str++;
    }

    return result;
}

// Функция для извлечения метки из строки
int extract_label(const char *line)
{
    while (*line && isspace(*line))
    {
        line++;
    }
    if (isdigit(*line))
    {
        return my_atoi(line);
    }

    return -1;
}

// Функция для поиска метки в массиве меток
int find_label(Label labels[], int num_labels, int label)
{
    for (int i = 0; i < num_labels; i++)
    {
        if (labels[i].label == label)
        {
            return i;
        }
    }

    return -1;
}

// Функция для сравнения строки с заданной константой
int compare_with_constant(const char *ptr, const char *constant)
{
    while (*constant)
    {
        if (*ptr != *constant)
        {
            return 0;
        }

        ptr++;
        constant++;
    }

    return 1;
}

// Функция для добавления текста в буфер без использования strcat
void my_strcat(char *dest, const char *src)
{
    while (*dest)
    {
        dest++;
    }
    while (*src)
    {
        *dest = *src;
        dest++;
        src++;
    }

    *dest = '\0';
}

// Функция для добавления ссылки на метку
void add_reference(Label labels[], int num_labels, const char *line)
{
    const char *ptr = line;
    int found_references = 0;
    char reference_buffer[MAX_LINE_LENGTH] = "";

    while (*ptr)
    {
        if (isdigit(*ptr))
        {
            const char *prev = ptr - 1;

            while (prev > line && isspace(*prev))
            {
                prev--;
            }
            if (prev > line && (*prev == ';' || *prev == ' ' || *prev == '\n'))
            {
                int label = my_atoi(ptr);
                int index = find_label(labels, num_labels, label);

                if (index != -1)
                {
                    labels[index].is_referenced = 1;

                    if (found_references)
                    {
                        my_strcat(reference_buffer, " or ");
                    }

                    char buffer[10];
                    int len = snprintf(buffer, sizeof(buffer), "%d", label);

                    if (len >= 0 && len < sizeof(buffer))
                    {
                        buffer[len] = '\0';
                        my_strcat(reference_buffer, buffer);

                        found_references = 1;
                    }
                }
                else
                {
                    if (found_references)
                    {
                        my_strcat(reference_buffer, " or ");
                    }

                    my_strcat(reference_buffer, "non-existing ");

                    char buffer[10];
                    int len = snprintf(buffer, sizeof(buffer), "%d", label);

                    if (len >= 0 && len < sizeof(buffer))
                    {
                        buffer[len] = '\0';
                        my_strcat(reference_buffer, buffer);

                        found_references = 1;
                    }
                }
            }
            while (isdigit(*ptr))
            {
                ptr++;
            }
        }
        else if (compare_with_constant(ptr, "then") || compare_with_constant(ptr, "else"))
        {
            ptr += 4;

            while (*ptr && isspace(*ptr))
            {
                ptr++;
            }
            if (isdigit(*ptr))
            {
                int label = my_atoi(ptr);
                int index = find_label(labels, num_labels, label);

                if (index != -1)
                {
                    labels[index].is_referenced = 1;

                    if (found_references)
                    {
                        my_strcat(reference_buffer, " or ");
                    }

                    char buffer[10];
                    int len = snprintf(buffer, sizeof(buffer), "%d", label);

                    if (len >= 0 && len < sizeof(buffer))
                    {
                        buffer[len] = '\0';
                        my_strcat(reference_buffer, buffer);

                        found_references = 1;
                    }
                }
                else
                {
                    if (found_references)
                    {
                        my_strcat(reference_buffer, " or ");
                    }

                    my_strcat(reference_buffer, "non-existing ");

                    char buffer[10];
                    int len = snprintf(buffer, sizeof(buffer), "%d", label);

                    if (len >= 0 && len < sizeof(buffer))
                    {
                        buffer[len] = '\0';
                        my_strcat(reference_buffer, buffer);

                        found_references = 1;
                    }
                }
                while (isdigit(*ptr))
                {
                    ptr++;
                }
            }
        }
        else
        {
            ptr++;
        }
    }
    if (found_references)
    {
        printf("Found reference to label: %s\n", reference_buffer);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    #pragma warning(push)
    #pragma warning(disable : 4996)
    FILE *file = fopen(argv[1], "r");
    #pragma warning(pop)

    if (!file)
    {
        perror("Error opening file!");
        return 1;
    }

    int num_labels = 0;
    Label labels[MAX_LABELS] = {};
    char line[MAX_LINE_LENGTH] = {};

    // Сбор всех меток
    printf("Collecting labels:\n");
    while (fgets(line, sizeof(line), file))
    {
        int label = extract_label(line);

        if (label != -1)
        {
            labels[num_labels].label = label;
            labels[num_labels].is_referenced = 0;
            printf("Found label: %d\n", label);
            num_labels++;
        }
    }

    // Поиск ссылок на метки
    printf("\nFinding references to labels:\n");
    fseek(file, 0, SEEK_SET);
    while (fgets(line, sizeof(line), file))
    {
        add_reference(labels, num_labels, line);
    }

    fclose(file);

    // Печать меток, на которые нет ссылок
    printf("\nLabels with no references:\n");
    for (int i = 0; i < num_labels; i++)
    {
        if (!labels[i].is_referenced)
        {
            printf("%d\n", labels[i].label);
        }
    }
}
