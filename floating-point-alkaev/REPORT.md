| Лабораторная работа №1 | 22Б05|  Архитектура компьютера |
| ---------------------- | ----------- | ---- |
| Представление чисел    |Алькаев Равиль Юрисович      | 2023 (ППА2) |

## Инструментарий
> Программа написана на C++ (11.4.0). Выполнено в среде разработки VS Code.

## Что реализовано
> Написал программу, которая позволяет выполнять арифметические действия с дробными числами в формате плавающая точка. Программа использует только целочисленные вычисления и типы данных.

## Константы

> Для работы программы нам нужны будут константы, они инициализированы в файле constant.h
> - chars16_to_2 - словарь, который нужен для перевода из 16-ричной системы счисления в массив из 4 битов
> - Rounding_* - типы округлений
> - hex_string - шестнадцатиричная строка (маленькие буковки)
> - index_to_hex массив шестнадцатиричных символов (большие буквы)
> - Types_* тип числа (+/- inf, nan и обычное)
## Функции 
> Для работы программы нам понадобятся некоторые вспомогательные функции (Все они реализованы в help_function.cpp)
> - is_valid() - проверка на правильность входной строки (является ли она кодом числа в 16-ричной записи)
> - hex_to_binary_code() - функция для перевода из 16-ричной в 2-ичную систему
> - integer_to_bin() функция, которая из integer получает code
> - bin_to_integer() перевод из 2-ичной в 10-тичную
> - increase() и decrease() увеличение или уменьшение числа на 1 соотвественно (зависит от округления)


## класс FloatPoint

> Все наши числа хранятся в классе FloatPoint, и все операции выполнены (+,-,*,/) над классом
```
class FloatPoint {
private:
    int exponent_len;
    int mantissa_len;
    int max_exponent;
    int min_exponent;
    int len_code;
    int shift_exponent;
    int bytes_out;
    int rounding;
    bool h_or_f;
    bool denormalized;
    int exp;
    int integer;
    std::vector<int> mant;
    std::vector<int> exp_code;
    int type;
    int sign;
}
 ```

> - h_or_f - half или float В самом начале нам нужно понять какого типа у нас число, исходя из этого можно много про него узнать

 ## что за что отвечает
> - exponent_len - длина экспонеты (для h 5, для f 8)
> - mantissa_len - длина мантиссы (для h 10, для f 23)
> - len_code - бит для числа (для h 16, для f 32)
> - shift_exponent - сдвиг экспоненты (для h 15, для f 127)
> - max_exponent - (для h 16, для f 128)
> - min_exponent - (для h -14, для f -126)
> - bytes_out - 16-ричных символов для мантиссы (для h 3, для f 6)
> - rounding - тип округления
> - denormalized - денормализованно ли оно
> - exp - значение экспоненты
> - integer - переводим мантиссу в целое число
> - mant - вектор кодирующий мантиссу (разные случаи для нормализованых и денормализованных чисел)
> - exp_code - вектор кодирующий экспоненту
> - type - тип числа (+/- inf, nan и обычное)
> - sign - знак нашего числа


## Операции над классом

###  Сложение 
> - Сначала разбираем частные случаи (nan, +-inf, 0) 
> - Теперь также разбираем задачу на случаи, но более общие
>> - Сложение двух денормализованных: складываем их значение integer, если получаем число большее максимальному ($2^{mantissa_len}$), то имеем нормализованное с минимальной экспонентой, иначу все также денормализованное 
>> - Нормализованое с денормализованым: если у нормализованного экспонента больше хотя бы на mantissa_len + 2, то ответ в зависимости от округления равен этому, следующему либо предыдущему числу, в противоположном случае преобразуем к виду $2^{min_exponent-mantissa_len}·integer$ складываем и переводим обратно в плавающую
>> - Оба нормализованных, действия аналогичны прошлому пункту, если экспоненты сильно отличаются делаем первое, иначе приводим к меньшей степени скалдываем и переводим обратно

### Вычитание 
> - Сначала разбираем частные случаи (nan, +-inf, 0) 
> - для вычитание используется функция to_opposite() превращает число в противоположное, после этого это равносильно сложению

### Умножение
> - Сначала разбираем частные случаи (nan, +-inf, 0) 
> - Далее переведем наши числа к нормальному виду $2^a * b$ (b от 1 до 2) экспонента может поменяться и при перемножение мантисс, но максимум на 1, на это будем обращать внимание, перемножая мантиссу получим больше битиков, нужно будет отбросить последние обращая внимание на округление 

### Деление
> - Сначала разбираем частные случаи (nan, +-inf, 0) 
> - Далее переведем наши числа к нормальному виду $2^a * b$ (b от 1 до 2), отношение bшек считаем как частное $b_1 * 2^{x + 1} / b_2 $
где x = mantissa_len, сначала переводим bшки в целое и переводим к виду $2^a * b$, возможно нам опять понадобиться подправить экспоненту исходя из вычисления мантисс, после переводим в плавающую точку

### Вывод чисел
> Также как и во всех операциях сначала занимаемся частными случаями, а дальше выводим в 16-ричной показательной форме с 10-тичной степенью

### Также в классе были реализованны другие функции, которые помогали главным, перечислем их:
> - to_plus_inf() - создает +inf
> - to_minus_inf() - создает -inf
> - to_inf_with_sign() - получает знак, и создает inf со знаком
> - to_nan() - создает nan
> - to_null() - создает +0
> - to_null_with_sign() - получает знак, и создает 0 со знаком
> - to_opposite() - создает противоположное число
> - increase() - выдает наименьшее число, которое больше даного 
> - decrease() - выдает наибольшее число, которое меньше даного
> - rounding_result() - зная знак  и экспоненту числа окргуляет code до величины мантиссы ссылаясь на округление, и после получаем число в формате плавающая точка
> - to_ans() - на входе знак, значение экспоненты и вектор из 0 и 1 отвечающий за мантиссу, создаем число в формате плавающая точка
> - res_to_small() - Если результат операции двух ненулевых чисел очень мал по модулю, выдаем либо 0, либо минимальное по модулю положительное или отрицательное (в зависимости от знака) число с плавающей точкой
> - biggest_positive() - +inf или наибольшее положительное число в зависимости от окгруления
> - biggest_negative() - -inf или наибольшее (по модулю) отрицательное число в зависимости от округления
> - biggest_with_sign() - тоже, что и прошлые два, но мы сами выбираем знак
> - is_null() - не ноль ли это


## main.cpp
> В главном файле, имеется функция solve(), которая занимается парсингом и выбором нужной операции, а также есть функция splitString(), которая считывает строку и преобразует ее в массив

## Результаты на тестовых данных

| Ввод | Вывод |
|------|-------|
|f 0 0xB9CD542|0x1.39aa84p-104 0x0B9CD542|
|f 0 0x8B9CD542|-0x1.39aa84p-104 0x8B9CD542|
|h 1 0x4145 * 0x142eB|0x1.238p+3 0x488F|
|f 2 0x1 / 0x0| inf 0x7F800000|
|h 3 0xFF10 + 0x7F01 | nan 0xFF10|
|h 1 0x4145| 0x1.514p+1 0x4145|
|h 1 0x4145 + 0x3AAA|0x1.bc0p+1 0x42F0|
|f 3 0x21ffac3 / 0xff32ffaa|-0x1.000000p-149 0x80000001|
|f 1 0x4142222 * 0x42ebfa11|0x1.0c2f00p-132 0x0002185E|
|f 3 0x21ffac3 - 0xff32ffaa|0x1.65ff54p+127 0x7F32FFAA|
|f 3 0x21ffac3 + 0xff32ffaa|-0x1.65ff56p+127 0xFF32FFAB|
|h 1 0x21ff + 0xfaee|-0x1.bb8p+15 0xFAEE|
|h 1 0x21ff * 0xfaee|-0x1.4c8p+9 0xE132|
|h 1 0x21ff / 0xfaee|-0x1.800p-23 0x8003|