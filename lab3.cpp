#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <windows.h>
#include <algorithm>
#include <chrono>
#include <cstdlib>

using namespace std;

const int maxw = 50;
const int maxlen = 10;

const int maxpos = 100000;
const int maxpat = 1000;

int plin[maxpos];
int pkmp[maxpos];
int pbm[maxpos];

bool isPunct(char c) {
    return c == '.' || c == ',' || c == ';' || c == ':' ||
           c == '!' || c == '?' || c == '-';
}

bool isLetterOrDigit(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9');
}

bool isLetter(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z');
}

bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

char toLowerChar(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c - 'A' + 'a';
    }
    return c;
}

string toLowerString(string s) {
    for (int i = 0; i < (int)s.size(); i++) {
        s[i] = toLowerChar(s[i]);
    }
    return s;
}

string readFromKeyboard() {
    string txt;
    cout << "Введите последовательность слов (в конце точка):" << endl;
    getline(cin, txt);
    return txt;
}

string readFromFile(const string& fname) {
    ifstream file(fname);
    string txt = "";

    if (!file.is_open()) {
        cout << "Не удалось открыть файл " << fname << endl;
        return txt;
    }

    string line;
    while (getline(file, line)) {
        txt += line + " ";
    }
    file.close();
    return txt;
}

int splitToWords(const string& txt, string w[]) {
    int cnt = 0;
    string cur = "";

    for (int i = 0; i < (int)txt.size(); i++) {
        char c = txt[i];

        if (isLetterOrDigit(c)) {
            cur += c;
        } else {
            if (cur != "") {
                if (cnt < maxw) {
                    w[cnt] = cur;
                }
                cnt++;
                cur = "";
            }
        }
    }

    if (cur != "") {
        if (cnt < maxw) {
            w[cnt] = cur;
        }
        cnt++;
    }

    return cnt;
}

string removeExtraSpaces(const string& txt) {
    string res = "";
    int n = txt.size();

    for (int i = 0; i < n; i++) {
        if (txt[i] != ' ') {
            res += txt[i];
            continue;
        }

        if (res == "" || res.back() == ' ') {
            continue;
        }

        int j = i;
        while (j < n && txt[j] == ' ') {
            j++;
        }

        if (j == n || txt[j] == '.') {
            continue;
        }

        res += ' ';
    }

    return res;
}

string removeExtraPunctuation(const string& txt) {
    string res = "";
    int n = txt.size();
    int i = 0;

    while (i < n) {
        if (!isPunct(txt[i])) {
            res += txt[i];
            i++;
            continue;
        }

        int j = i;
        while (j < n && isPunct(txt[j])) {
            j++;
        }
        string grp = txt.substr(i, j - i);

        if (grp == "...") {
            res += grp;
        } else {
            res += grp[0];
        }

        i = j;
    }

    return res;
}

string fixCase(string txt) {
    int n = txt.size();
    int i = 0;

    while (i < n) {
        if (!isLetterOrDigit(txt[i])) {
            i++;
            continue;
        }

        int st = i;
        while (i < n && isLetterOrDigit(txt[i])) {
            i++;
        }
        int en = i;

        if (txt[st] >= 'a' && txt[st] <= 'z') {
            txt[st] = txt[st] - 'a' + 'A';

            for (int k = st + 1; k < en; k++) {
                if (txt[k] >= 'A' && txt[k] <= 'Z') {
                    txt[k] = txt[k] - 'A' + 'a';
                }
            }
        }
    }

    return txt;
}

bool checkInput(const string& txt) {
    int last = txt.size() - 1;
    while (last >= 0 && txt[last] == ' ') {
        last--;
    }
    if (last < 0 || txt[last] != '.') {
        cout << "Ошибка: после последнего слова должна стоять точка." << endl;
        return false;
    }

    for (int i = 0; i < (int)txt.size(); i++) {
        char c = txt[i];

        if (!isLetterOrDigit(c) && c != ' ' && !isPunct(c)) {
            cout << "Ошибка: допустимы только латинские буквы и цифры." << endl;
            return false;
        }
    }

    string w[maxw];
    int cnt = splitToWords(txt, w);

    if (cnt < 1 || cnt > maxw) {
        cout << "Ошибка: слов должно быть от 1 до " << maxw
             << ", а введено " << cnt << "." << endl;
        return false;
    }

    for (int i = 0; i < cnt; i++) {
        if ((int)w[i].size() > maxlen) {
            cout << "Ошибка: слово \"" << w[i]
                 << "\" длиннее " << maxlen << " символов." << endl;
            return false;
        }
    }

    return true;
}

bool hasDigit(const string& s) {
    for (int i = 0; i < (int)s.size(); i++) {
        if (isDigit(s[i])) {
            return true;
        }
    }
    return false;
}

bool hasLetter(const string& s) {
    for (int i = 0; i < (int)s.size(); i++) {
        if (isLetter(s[i])) {
            return true;
        }
    }
    return false;
}

bool hasRepeatedLetter(const string& s) {
    for (int i = 0; i < (int)s.size(); i++) {
        if (!isLetter(s[i])) {
            continue;
        }

        for (int j = i + 1; j < (int)s.size(); j++) {
            if (toLowerChar(s[i]) == toLowerChar(s[j])) {
                return true;
            }
        }
    }
    return false;
}

bool firstLetterAgain(const string& s) {
    if (s.size() == 0) {
        return false;
    }
    if (!isLetter(s[0])) {
        return false;
    }

    char first = toLowerChar(s[0]);

    for (int i = 1; i < (int)s.size(); i++) {
        if (toLowerChar(s[i]) == first) {
            return true;
        }
    }
    return false;
}

int reverseWords(const string w[], int cnt, string res[]) {
    int k = 0;

    for (int i = cnt - 1; i >= 0; i--) {
        res[k] = w[i];
        k++;
    }

    return k;
}

int sortWords(const string w[], int cnt, string res[]) {
    for (int i = 0; i < cnt; i++) {
        res[i] = w[i];
    }

    for (int i = 0; i < cnt - 1; i++) {
        for (int j = 0; j < cnt - 1 - i; j++) {
            string x = toLowerString(res[j]);
            string y = toLowerString(res[j + 1]);

            if (x > y) {
                string tmp = res[j];
                res[j] = res[j + 1];
                res[j + 1] = tmp;
            }
        }
    }

    return cnt;
}

int wordsWithoutDigits(const string w[], int cnt, string res[]) {
    int k = 0;

    for (int i = 0; i < cnt; i++) {
        if (!hasDigit(w[i])) {
            res[k] = w[i];
            k++;
        }
    }

    return k;
}

int wordsWithRepeatedLetters(const string w[], int cnt, string res[]) {
    int k = 0;

    for (int i = 0; i < cnt; i++) {
        if (hasRepeatedLetter(w[i])) {
            res[k] = w[i];
            k++;
        }
    }

    return k;
}

int wordsFirstLetterAgain(const string w[], int cnt, string res[]) {
    int k = 0;

    for (int i = 0; i < cnt; i++) {
        if (firstLetterAgain(w[i])) {
            res[k] = w[i];
            k++;
        }
    }

    return k;
}

void groupWords(const string w[], int cnt) {
    int n = 0;

    cout << "Только буквы:  ";
    n = 0;
    for (int i = 0; i < cnt; i++) {
        if (hasLetter(w[i]) && !hasDigit(w[i])) {
            cout << w[i] << " ";
            n++;
        }
    }
    if (n == 0) {
        cout << "-";
    }
    cout << endl;

    cout << "Только цифры:  ";
    n = 0;
    for (int i = 0; i < cnt; i++) {
        if (hasDigit(w[i]) && !hasLetter(w[i])) {
            cout << w[i] << " ";
            n++;
        }
    }
    if (n == 0) {
        cout << "-";
    }
    cout << endl;

    cout << "Буквы + цифры: ";
    n = 0;
    for (int i = 0; i < cnt; i++) {
        if (hasLetter(w[i]) && hasDigit(w[i])) {
            cout << w[i] << " ";
            n++;
        }
    }
    if (n == 0) {
        cout << "-";
    }
    cout << endl;
}

void printWords(const string w[], int cnt) {
    if (cnt == 0) {
        cout << "Таких слов нет." << endl;
        return;
    }

    for (int i = 0; i < cnt; i++) {
        cout << w[i] << " ";
    }
    cout << endl;
}

int firstToUpper(const string w[], int cnt, string res[]) {
    for (int i = 0; i < cnt; i++) {
        res[i] = w[i];

        if (res[i].size() > 0) {
            if (res[i][0] >= 'a' && res[i][0] <= 'z') {
                res[i][0] = res[i][0] - 'a' + 'A';
            }
        }
    }

    return cnt;
}

void showLengths(const string w[], int cnt) {
    for (int i = 0; i < cnt; i++) {
        cout << w[i] << " - " << (int)w[i].size() << endl;
    }
}

int removeChars(const string w[], int cnt, const string& chars, string res[]) {
    int k = 0;

    for (int i = 0; i < cnt; i++) {
        string nw = "";

        for (int j = 0; j < (int)w[i].size(); j++) {
            bool rem = false;

            for (int m = 0; m < (int)chars.size(); m++) {
                if (toLowerChar(w[i][j]) == toLowerChar(chars[m])) {
                    rem = true;
                }
            }

            if (!rem) {
                nw += w[i][j];
            }
        }

        if (nw != "") {
            res[k] = nw;
            k++;
        }
    }

    return k;
}

int digitsToLetters(const string w[], int cnt, string res[]) {
    for (int i = 0; i < cnt; i++) {
        string nw = "";

        for (int j = 0; j < (int)w[i].size(); j++) {
            char c = w[i][j];

            if (c >= '1' && c <= '9') {
                nw += (char)('a' + (c - '1'));
            } else {
                nw += c;
            }
        }

        res[i] = nw;
    }

    return cnt;
}

int digitsToEnd(const string w[], int cnt, string res[]) {
    for (int i = 0; i < cnt; i++) {
        string let = "";
        string dig = "";

        for (int j = 0; j < (int)w[i].size(); j++) {
            char c = w[i][j];

            if (isDigit(c)) {
                dig += c;
            } else {
                let += c;
            }
        }

        res[i] = let + dig;
    }

    return cnt;
}

void printVertical(const string w[], int cnt) {
    for (int i = 0; i < cnt; i++) {
        for (int j = 0; j < (int)w[i].size(); j++) {
            cout << w[i][j] << endl;
        }
        cout << endl;
    }
}

void printInColumns(const string w[], int cnt, int cols) {
    int mx = 0;
    for (int i = 0; i < cnt; i++) {
        if ((int)w[i].size() > mx) {
            mx = w[i].size();
        }
    }

    int i = 0;
    while (i < cnt) {
        for (int col = 0; col < cols; col++) {
            if (i >= cnt) {
                break;
            }

            int sp = mx - (int)w[i].size();
            for (int s = 0; s < sp; s++) {
                cout << " ";
            }

            cout << w[i] << "  ";
            i++;
        }
        cout << endl;
    }
}

void printMenu3() {
    cout << endl;
    cout << "===== ПРЕОБРАЗОВАНИЕ СЛОВ =====" << endl;
    cout << "1 - первая буква прописная" << endl;
    cout << "2 - длина каждого слова" << endl;
    cout << "3 - удалить заданные символы" << endl;
    cout << "4 - заменить цифры на буквы" << endl;
    cout << "5 - перенести цифры в конец слова" << endl;
    cout << "6 - вертикальный вывод слов" << endl;
    cout << "7 - вывод в 2-3 колонки" << endl;
    cout << "0 - назад" << endl;
    cout << "Ваш выбор: ";
}

void transformMenu(const string w[], int cnt) {
    while (true) {
        int ch3;
        string res[maxw];
        int rc = 0;

        printMenu3();
        cin >> ch3;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Нужно ввести число из меню." << endl;
            continue;
        }
        cin.ignore(10000, '\n');

        switch (ch3) {
            case 0:
            {
                return;
            }

            case 1:
            {
                rc = firstToUpper(w, cnt, res);
                cout << "Первая буква прописная: ";
                printWords(res, rc);
                break;
            }

            case 2:
            {
                showLengths(w, cnt);
                break;
            }

            case 3:
            {
                string chars;
                cout << "Какие символы удалить (например, a1): ";
                getline(cin, chars);

                rc = removeChars(w, cnt, chars, res);
                cout << "После удаления: ";
                printWords(res, rc);
                break;
            }

            case 4:
            {
                rc = digitsToLetters(w, cnt, res);
                cout << "Цифры заменены на буквы: ";
                printWords(res, rc);
                break;
            }

            case 5:
            {
                rc = digitsToEnd(w, cnt, res);
                cout << "Цифры в конце слов: ";
                printWords(res, rc);
                break;
            }

            case 6:
            {
                printVertical(w, cnt);
                break;
            }

            case 7:
            {
                int cols;
                cout << "Сколько колонок (2 или 3): ";
                cin >> cols;
                cin.ignore(10000, '\n');

                if (cols != 2 && cols != 3) {
                    cout << "Нужно 2 или 3 колонки." << endl;
                    break;
                }

                printInColumns(w, cnt, cols);
                break;
            }

            default:
            {
                cout << "Такого пункта нет." << endl;
                break;
            }
        }
    }
}

int searchLinear(const string& txt, const string& pat, int pos[]) {
    int n = txt.size();
    int m = pat.size();
    int cnt = 0;

    if (m == 0 || m > n) {
        return 0;
    }

    for (int i = 0; i <= n - m; i++) {
        bool same = true;

        for (int j = 0; j < m; j++) {
            if (txt[i + j] != pat[j]) {
                same = false;
                break;
            }
        }

        if (same) {
            if (cnt < maxpos) {
                pos[cnt] = i;
            }
            cnt++;
        }
    }

    return cnt;
}

void computeLPSArray(const string& pat, int lps[]) {
    int m = pat.size();
    int len = 0;
    lps[0] = 0;
    int i = 1;

    while (i < m) {
        if (pat[i] == pat[len]) {
            len++;
            lps[i] = len;
            i++;
        } else {
            if (len != 0) {
                len = lps[len - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
}

int KMPSearch(const string& txt, const string& pat, int pos[]) {
    int n = txt.size();
    int m = pat.size();
    int cnt = 0;

    if (m == 0 || m > n || m > maxpat) {
        return 0;
    }

    int lps[maxpat];
    computeLPSArray(pat, lps);

    int i = 0;
    int j = 0;

    while (i < n) {
        if (pat[j] == txt[i]) {
            i++;
            j++;
        }

        if (j == m) {
            if (cnt < maxpos) {
                pos[cnt] = i - j;
            }
            cnt++;
            j = lps[j - 1];
        } else if (i < n && pat[j] != txt[i]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }

    return cnt;
}


void buildBadCharTable(const string& pat, int bad[]) {
    for (int i = 0; i < 256; i++) {
        bad[i] = -1;
    }

    for (int i = 0; i < (int)pat.size(); i++) {
        bad[(unsigned char)pat[i]] = i;
    }
}

int boyerMooreSearch(const string& txt, const string& pat, int pos[]) {
    int n = txt.size();
    int m = pat.size();
    int cnt = 0;

    if (m == 0 || m > n) {
        return 0;
    }

    int bad[256];
    buildBadCharTable(pat, bad);

    int s = 0;
    while (s <= n - m) {
        int j = m - 1;

        while (j >= 0 && pat[j] == txt[s + j]) {
            j--;
        }

        if (j < 0) {
            if (cnt < maxpos) {
                pos[cnt] = s;
            }
            cnt++;

            if (s + m < n) {
                s += m - bad[(unsigned char)txt[s + m]];
            } else {
                s += 1;
            }
        } else {
            int shift = max(1, j - bad[(unsigned char)txt[s + j]]);
            s += shift;
        }
    }

    return cnt;
}


void printPositions(const string& name, const int pos[], int cnt) {
    cout << name << ": ";

    if (cnt == 0) {
        cout << "не найдено" << endl;
        return;
    }

    cout << "найдено " << cnt << ", позиции: ";

    int shown = cnt;
    if (shown > maxpos) {
        shown = maxpos;
    }

    for (int i = 0; i < shown; i++) {
        cout << pos[i] << " ";
    }
    cout << endl;
}

void showSearch(const string& txt, const string& pat) {
    if (pat.size() == 0) {
        cout << "Образец пустой." << endl;
        return;
    }
    if ((int)pat.size() > maxpat) {
        cout << "Образец слишком длинный (максимум " << maxpat << ")." << endl;
        return;
    }
    if (pat.size() > txt.size()) {
        cout << "Образец длиннее текста." << endl;
        return;
    }

    int c1 = searchLinear(txt, pat, plin);
    int c2 = KMPSearch(txt, pat, pkmp);
    int c3 = boyerMooreSearch(txt, pat, pbm);

    cout << "Текст: \"" << txt << "\"" << endl;
    cout << "Образец: \"" << pat << "\" (позиции считаются с 0)" << endl;
    printPositions("Линейный поиск", plin, c1);
    printPositions("КМП           ", pkmp, c2);
    printPositions("Бойер-Мур     ", pbm, c3);
}

string makeRandomText(int len, int alpha) {
    string res = "";

    for (int i = 0; i < len; i++) {
        res += (char)('a' + rand() % alpha);
    }

    return res;
}

bool samePositions(const int a[], int ca, const int b[], int cb) {
    if (ca != cb) {
        return false;
    }

    for (int i = 0; i < ca && i < maxpos; i++) {
        if (a[i] != b[i]) {
            return false;
        }
    }

    return true;
}

double measureTime(int alg, const string& txt, const string& pat, int runs) {
    chrono::steady_clock::time_point st = chrono::steady_clock::now();

    for (int r = 0; r < runs; r++) {
        if (alg == 1) {
            searchLinear(txt, pat, plin);
        } else if (alg == 2) {
            KMPSearch(txt, pat, pkmp);
        } else {
            boyerMooreSearch(txt, pat, pbm);
        }
    }

    chrono::steady_clock::time_point fin = chrono::steady_clock::now();

    double total = chrono::duration_cast<chrono::microseconds>(fin - st).count();

    return total / runs;
}

void runComparison() {
    srand(12345);

    const int tests = 5;

    int tlen[tests] = {1000, 10000, 100000, 100000, 10000};
    int plen[tests] = {5, 10, 20, 100, 10};
    int nruns[tests] = {1000, 200, 20, 20, 100};

    cout << fixed;
    cout.precision(2);

    for (int t = 0; t < tests; t++) {
        string txt;
        string pat;

        if (t == 4) {
            txt = string(tlen[t], 'a');
            pat = string(plen[t] - 1, 'a') + "b";
        } else {
            txt = makeRandomText(tlen[t], 4);

            int st = rand() % (tlen[t] - plen[t] + 1);
            pat = txt.substr(st, plen[t]);
        }

        int c1 = searchLinear(txt, pat, plin);
        int c2 = KMPSearch(txt, pat, pkmp);
        int c3 = boyerMooreSearch(txt, pat, pbm);

        cout << endl;
        cout << "Тест " << (t + 1) << ": текст " << tlen[t]
             << " символов, образец " << plen[t]
             << " символов, прогонов " << nruns[t];
        if (t == 4) {
            cout << " (тяжёлый случай)";
        }
        cout << endl;

        cout << "  Найдено вхождений: " << c1 << endl;

        if (!samePositions(plin, c1, pkmp, c2) ||
            !samePositions(plin, c1, pbm, c3)) {
            cout << "  ВНИМАНИЕ: алгоритмы нашли разные позиции!" << endl;
        }

        double d1 = measureTime(1, txt, pat, nruns[t]);
        double d2 = measureTime(2, txt, pat, nruns[t]);
        double d3 = measureTime(3, txt, pat, nruns[t]);

        cout << "  Линейный поиск: " << d1 << " мкс" << endl;
        cout << "  КМП:            " << d2 << " мкс" << endl;
        cout << "  Бойер-Мур:      " << d3 << " мкс" << endl;
    }

    cout.unsetf(ios::fixed);
}

void printMenu() {
    cout << endl;
    cout << "===== МЕНЮ =====" << endl;
    cout << "1 - ввести текст с клавиатуры" << endl;
    cout << "2 - прочитать текст из файла" << endl;
    cout << "0 - выход" << endl;
    cout << "Ваш выбор: ";
}

void printMenu2() {
    cout << endl;
    cout << "===== ЗАДАНИЯ СО СЛОВАМИ =====" << endl;
    cout << "1 - слова в обратном порядке" << endl;
    cout << "2 - слова в алфавитном порядке" << endl;
    cout << "3 - слова без цифр" << endl;
    cout << "4 - слова с повторяющимися буквами" << endl;
    cout << "5 - слова, где первая буква встречается ещё раз" << endl;
    cout << "6 - группировка (буквы / цифры / буквы+цифры)" << endl;
    cout << "7 - задания на преобразование слов (этап 5)" << endl;
    cout << "8 - поиск подстроки в тексте (этап 6)" << endl;
    cout << "9 - сравнение алгоритмов поиска (этап 7)" << endl;
    cout << "10 - удалить дубликаты слов" << endl;
    cout << "0 - назад" << endl;
    cout << "Ваш выбор: ";
}

void wordsMenu(const string w[], int cnt, const string& txt) {
    while (true) {
        int ch2;
        string res[maxw];
        int rc = 0;

        printMenu2();
        cin >> ch2;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Нужно ввести число из меню." << endl;
            continue;
        }
        cin.ignore(10000, '\n');

        switch (ch2) {
            case 0:
            {
                return;
            }

            case 1:
            {
                rc = reverseWords(w, cnt, res);
                cout << "В обратном порядке: ";
                printWords(res, rc);
                break;
            }

            case 2:
            {
                rc = sortWords(w, cnt, res);
                cout << "В алфавитном порядке: ";
                printWords(res, rc);
                break;
            }

            case 3:
            {
                rc = wordsWithoutDigits(w, cnt, res);
                cout << "Слова без цифр: ";
                printWords(res, rc);
                break;
            }

            case 4:
            {
                rc = wordsWithRepeatedLetters(w, cnt, res);
                cout << "Слова с повторяющимися буквами: ";
                printWords(res, rc);
                break;
            }

            case 5:
            {
                rc = wordsFirstLetterAgain(w, cnt, res);
                cout << "Слова, где первая буква встречается ещё раз: ";
                printWords(res, rc);
                break;
            }

            case 6:
            {
                groupWords(w, cnt);
                break;
            }

            case 7:
            {
                transformMenu(w, cnt);
                break;
            }

            case 8:
            {
                string pat;
                cout << "Введите образец для поиска: ";
                getline(cin, pat);
                showSearch(txt, pat);
                break;
            }

            case 9:
            {
                runComparison();
                break;
            }

            case 10:
            {
                rc = 0;

                for (int i = 0; i < cnt; i++) {
                    bool have = false;

                    for (int j = 0; j < rc; j++) {
                        if (res[j] == w[i]) {
                            have = true;
                        }
                    }

                    if (!have) {
                        res[rc] = w[i];
                        rc++;
                    }
                }

                cout << "Без дубликатов: ";
                printWords(res, rc);
                break;
            }

            default:
            {
                cout << "Такого пункта нет." << endl;
                break;
            }
        }
    }
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    while (true) {
        int ch;
        string txt = "";

        printMenu();
        cin >> ch;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Нужно ввести число из меню." << endl;
            continue;
        }
        cin.ignore(10000, '\n');

        switch (ch) {
            case 0:
            {
                return 0;
            }

            case 1:
            {
                txt = readFromKeyboard();
                break;
            }

            case 2:
            {
                string fname;
                cout << "Имя файла: ";
                getline(cin, fname);
                txt = readFromFile(fname);
                break;
            }

            default:
            {
                cout << "Такого пункта нет." << endl;
                continue;
            }
        }

        if (!checkInput(txt)) {
            continue;
        }

        cout << endl << "Исходный текст:       \"" << txt << "\"" << endl;

        txt = removeExtraSpaces(txt);
        txt = removeExtraPunctuation(txt);
        txt = fixCase(txt);

        cout << "После редактирования: \"" << txt << "\"" << endl;

        string w[maxw];
        int cnt = splitToWords(txt, w);

        wordsMenu(w, cnt, txt);
    }
}