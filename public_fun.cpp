#include "public_fun.h"

bool isAlpha(const QString &text)
{
    QRegExp regex("^[a-zA-Z]+$");
    return regex.exactMatch(text);
}


QString incTailNumber(const QString &text)
{
    // 查找尾部最多18位连续数字
    QRegularExpression regex("(\\d{1,18})$");
    QRegularExpressionMatch match = regex.match(text);

    QString numberPart;
    if(match.hasMatch()) {
        numberPart = match.captured(1);
    }
    bool ok;
    qulonglong number = numberPart.toULongLong(&ok);

    if(!ok) {
        return text;
    }

    number++;

    // 使用新的数字替换原来的尾部数字部分
    QString incNumber = QString::number(number).rightJustified(numberPart.length(), '0');

    return text.left(text.length() - numberPart.length()) + incNumber;
}

QString incTailAlpha(const QString &text)
{
    // 查找尾部最多18位的连续字母
    QRegularExpression regexLower("([a-z]{1,18})$");
    QRegularExpression regexUpper("([A-Z]{1,18})$");
    QRegularExpressionMatch matchLower = regexLower.match(text);
    QRegularExpressionMatch matchUpper = regexUpper.match(text);

    QString lettersPart;

    if (matchLower.hasMatch()) {
        lettersPart = matchLower.captured(1);
    }
    else if(matchUpper.hasMatch()){
        lettersPart = matchUpper.captured(1);
    }
    else {
        return text; // 如果没有找到尾部字母，返回原字符串
    }

    // 获取尾部字母部分
    QString newLetters = lettersPart; // 准备存储新的字符串

    bool carry = true; // 进位标志，初始为true表示需要对最后一位进行加1操作
    for (int i = lettersPart.length() - 1; i >= 0 && carry; --i) {
        QChar ch = lettersPart[i];
        if (ch == 'z') {
            newLetters[i] = 'a'; // 'z' 进位变为 'a'
        } else if (ch == 'Z') {
            newLetters[i] = 'A'; // 'Z' 进位变为 'A'
        } else {
            newLetters[i] = ch.unicode() + 1; // 其他字符直接加1
            carry = false; // 处理完进位
        }
    }

    // 如果最高位仍有进位，需要在开头添加一个 'a' 或 'A'
    if (carry) {
        if (lettersPart[0].isLower()) {
            newLetters.prepend('a');
        } else {
            newLetters.prepend('A');
        }
    }

    // 返回原字符串的非字母部分 + 新的字母部分
    return text.left(text.length() - lettersPart.length()) + newLetters;
}

/*
QString decTailAlpha(const QString &text)
{
    // 查找尾部最多18位的连续字母
    QRegularExpression regexLower("([a-z]{1,18})$");
    QRegularExpression regexUpper("([A-Z]{1,18})$");
    QRegularExpressionMatch matchLower = regexLower.match(text);
    QRegularExpressionMatch matchUpper = regexUpper.match(text);

    QString lettersPart;

    if (matchLower.hasMatch()) {
        lettersPart = matchLower.captured(1);
    }
    else if(matchUpper.hasMatch()){
        lettersPart = matchUpper.captured(1);
    }
    else {
        return text; // 如果没有找到尾部字母，返回原字符串
    }

    QString newLetters = lettersPart; // 准备存储新的字符串

    bool borrow = true; // 借位标志，初始为true表示需要对最后一位进行减1操作
    for (int i = lettersPart.length() - 1; i >= 0 && borrow; --i) {
        QChar ch = lettersPart[i];
        if (ch == 'a') {
            newLetters[i] = 'z'; // 'z' 借位变为 'a'
        } else if (ch == 'A') {
            newLetters[i] = 'Z'; // 'Z' 借位变为 'A'
        } else {
            newLetters[i] = ch.unicode() - 1; // 其他字符直接减1
            borrow = false; // 处理完借位
        }
    }

    // 如果最高位仍要借位，需要删除最高位
    if (borrow) {
        newLetters.remove(0,1);
    }

    // 返回原字符串的非字母部分 + 新的字母部分
    return text.left(text.length() - lettersPart.length()) + newLetters;
}
*/

int calculateTotalCombinations(const QList<QStringList>& lists)
{
    int total = 1;
    for (const QStringList& list : lists)
    {
        total *= list.size();
    }
    return total;
}

void GenerateCombinations(const QList<QStringList>& lists, QStringList& result, QString currentCombination, int depth)
{
    if (depth == lists.size())
    {
        result.append(currentCombination);
        return;
    }
    for (const QString& item : lists[depth])
    {
        GenerateCombinations(lists, result, currentCombination + item + SPLIT_SYMBOL, depth + 1);
    }
}

QStringList generateNumberTailSequence(const QString text1, const QString text2)
{
    // 查找尾部最多18位连续数字
    QRegularExpression regex("(\\d{1,18})$");
    QRegularExpressionMatch match1 = regex.match(text1);
    QRegularExpressionMatch match2 = regex.match(text2);

    QString startStr, endStr;
    QString prefixStr1, prefixStr2;
    if(match1.hasMatch() && match2.hasMatch()) {
        prefixStr1 = text1.mid(0, text1.length() - match1.captured(1).length());
        prefixStr2 = text2.mid(0, text2.length() - match2.captured(1).length());
        startStr = match1.captured(1);
        endStr = match2.captured(1);
    }
    else {
        return QStringList();
    }

    if(prefixStr1 != prefixStr2)
    {
        return QStringList();
    }


    int min_len = qMin(startStr.length(), endStr.length());

    QStringList sequenceList;
    qulonglong startNum = startStr.toULongLong();
    qulonglong endNum = endStr.toULongLong();
    qulonglong currentNum;

    if(startNum <= endNum)
    {
        currentNum = startNum;
        while(currentNum <= endNum)
        {
            sequenceList.append(prefixStr1 + QString("%1").arg(currentNum, min_len, 10, QChar('0')));
            currentNum++;
        }
    }
    else
    {
        currentNum = endNum;
        while(currentNum <= startNum)
        {
            sequenceList.prepend(prefixStr1 + QString("%1").arg(currentNum, min_len, 10, QChar('0')));
            currentNum++;
        }
    }

    return sequenceList;
}


QStringList generateAlphaTailSequence(const QString text1, const QString text2)
{
    // 查找尾部最多18位的连续字母
    QRegularExpression regexLower("([a-z]{1,18})$");
    QRegularExpression regexUpper("([A-Z]{1,18})$");
    QRegularExpressionMatch matchLower1 = regexLower.match(text1);
    QRegularExpressionMatch matchLower2 = regexLower.match(text2);
    QRegularExpressionMatch matchUpper1 = regexUpper.match(text1);
    QRegularExpressionMatch matchUpper2 = regexUpper.match(text2);

    QString startStr, endStr;
    QString prefixStr1, prefixStr2;
    if (matchLower1.hasMatch() && matchLower2.hasMatch()) {
        prefixStr1 = text1.mid(0, text1.length() - matchLower1.captured(1).length());
        prefixStr2 = text2.mid(0, text2.length() - matchLower2.captured(1).length());
        startStr = matchLower1.captured(1);
        endStr = matchLower2.captured(1);
    }
    else if(matchUpper1.hasMatch() && matchUpper2.hasMatch()){
        prefixStr1 = text1.mid(0, text1.length() - matchUpper1.captured(1).length());
        prefixStr2 = text2.mid(0, text2.length() - matchUpper2.captured(1).length());
        startStr = matchUpper1.captured(1);
        endStr = matchUpper2.captured(1);
    }
    else {
        return QStringList();
    }

    if(prefixStr1 != prefixStr2)
    {
        return QStringList();
    }


    QStringList sequenceList;
    QString currentAlpha;
    if(lettersCompare(startStr, endStr) <= 0)
    {
        currentAlpha = startStr;
        while(lettersCompare(currentAlpha, endStr) <= 0)
        {
            sequenceList.append(prefixStr1 + currentAlpha);
            currentAlpha = incTailAlpha(currentAlpha);
        }
    }
    else
    {
        currentAlpha = endStr;
        while(lettersCompare(currentAlpha, startStr) <= 0)
        {
            sequenceList.prepend(prefixStr1 + currentAlpha);
            currentAlpha = incTailAlpha(currentAlpha);
        }
    }
    return sequenceList;
}

int lettersCompare(const QString letters1, const QString letters2)
{
    //对齐高低位
    int max_len = qMax(letters1.length(), letters2.length());
    QString alignedLetters1 = letters1.rightJustified(max_len, ' ');
    QString alignedLetters2 = letters2.rightJustified(max_len, ' ');

    return alignedLetters1.compare(alignedLetters2, Qt::CaseInsensitive);
}

QString decTailNumber(const QString &text)
{
    // 查找尾部最多18位连续数字
    QRegularExpression regex("(\\d{1,18})$");
    QRegularExpressionMatch match = regex.match(text);

    QString numberPart;
    if(match.hasMatch()) {
        numberPart = match.captured(1);
    }
    bool ok;
    qulonglong number = numberPart.toULongLong(&ok);

    if(!ok) {
        return text;
    }

    if(number>0)
        number--;

    // 使用新的数字替换原来的尾部数字部分
    QString incNumber = QString::number(number).rightJustified(numberPart.length(), '0');

    return text.left(text.length() - numberPart.length()) + incNumber;
}

QString decTailAlpha(const QString &text)
{
    // 查找尾部最多18位的连续字母
    QRegularExpression regexLower("([a-z]{1,18})$");
    QRegularExpression regexUpper("([A-Z]{1,18})$");
    QRegularExpressionMatch matchLower = regexLower.match(text);
    QRegularExpressionMatch matchUpper = regexUpper.match(text);

    QString lettersPart;

    if (matchLower.hasMatch()) {
        lettersPart = matchLower.captured(1);
    }
    else if(matchUpper.hasMatch()){
        lettersPart = matchUpper.captured(1);
    }
    else {
        return text; // 如果没有找到尾部字母，返回原字符串
    }

    // 如果减到最后一个，返回原字符串
    if(lettersPart == 'A' || lettersPart == 'a')
    {
        return text;
    }

    QString newLetters = lettersPart; // 准备存储新的字符串

    bool borrow = true; // 借位标志，初始为true表示需要对最后一位进行减1操作
    for (int i = lettersPart.length() - 1; i >= 0 && borrow; --i) {
        QChar ch = lettersPart[i];
        if (ch == 'a') {
            newLetters[i] = 'z'; // 'z' 借位变为 'a'
        } else if (ch == 'A') {
            newLetters[i] = 'Z'; // 'Z' 借位变为 'A'
        } else {
            newLetters[i] = ch.unicode() - 1; // 其他字符直接减1
            borrow = false; // 处理完借位
        }
    }

    // 如果最高位仍要借位，需要删除最高位
    if (borrow) {
        newLetters.remove(0,1);
    }

    // 返回原字符串的非字母部分 + 新的字母部分
    return text.left(text.length() - lettersPart.length()) + newLetters;
}

qulonglong countNumberTailSequence(const QString text1, const QString text2)
{
    // 查找尾部最多18位连续数字
    QRegularExpression regex("(\\d{1,18})$");
    QRegularExpressionMatch match1 = regex.match(text1);
    QRegularExpressionMatch match2 = regex.match(text2);

    QString startStr, endStr;
    QString prefixStr1, prefixStr2;
    if(match1.hasMatch() && match2.hasMatch()) {
        prefixStr1 = text1.mid(0, text1.length() - match1.captured(1).length());
        prefixStr2 = text2.mid(0, text2.length() - match2.captured(1).length());
        startStr = match1.captured(1);
        endStr = match2.captured(1);
    }
    else {
        return 0;
    }

    if(prefixStr1 != prefixStr2)
    {
        return 0;
    }

    qulonglong sequence_cnt = 0;
    qulonglong startNum = startStr.toULongLong();
    qulonglong endNum = endStr.toULongLong();

    if(startNum <= endNum)
    {
        sequence_cnt = endNum - startNum + 1;
    }
    else
    {
        sequence_cnt = startNum - endNum + 1;
    }

    return sequence_cnt;
}

qulonglong countAlphaTailSequence(const QString text1, const QString text2)
{
    // 查找尾部最多18位的连续字母
    QRegularExpression regexLower("([a-z]{1,18})$");
    QRegularExpression regexUpper("([A-Z]{1,18})$");
    QRegularExpressionMatch matchLower1 = regexLower.match(text1);
    QRegularExpressionMatch matchLower2 = regexLower.match(text2);
    QRegularExpressionMatch matchUpper1 = regexUpper.match(text1);
    QRegularExpressionMatch matchUpper2 = regexUpper.match(text2);

    QString startStr, endStr;
    QString prefixStr1, prefixStr2;
    if (matchLower1.hasMatch() && matchLower2.hasMatch()) {
        prefixStr1 = text1.mid(0, text1.length() - matchLower1.captured(1).length());
        prefixStr2 = text2.mid(0, text2.length() - matchLower2.captured(1).length());
        startStr = matchLower1.captured(1);
        endStr = matchLower2.captured(1);
    }
    else if(matchUpper1.hasMatch() && matchUpper2.hasMatch()){
        prefixStr1 = text1.mid(0, text1.length() - matchUpper1.captured(1).length());
        prefixStr2 = text2.mid(0, text2.length() - matchUpper2.captured(1).length());
        startStr = matchUpper1.captured(1);
        endStr = matchUpper2.captured(1);
    }
    else {
        return 0;
    }

    if(prefixStr1 != prefixStr2)
    {
        return 0;
    }

    qulonglong sequence_cnt = 0;
    qulonglong startStr_base_26_value = 0;
    qulonglong endStr_base_26_value = 0;

    for(QChar c : startStr)
    {
        startStr_base_26_value = startStr_base_26_value * 26 + (c.toLower().unicode() - 'a' + 1);
        qDebug() << "c.toLower().unicode() - 'a'" << c.toLower().unicode() - 'a';
    }
    for(QChar c : endStr)
    {
        endStr_base_26_value = endStr_base_26_value * 26 + (c.toLower().unicode() - 'a' + 1);
    }
    qDebug() << "startStr_base_26_value" << startStr_base_26_value;
    qDebug() << "endStr_base_26_value" << endStr_base_26_value;

    if(startStr_base_26_value <= endStr_base_26_value)
    {
        sequence_cnt = endStr_base_26_value - startStr_base_26_value + 1;
    }
    else
    {
        sequence_cnt = startStr_base_26_value - endStr_base_26_value + 1;
    }

    return sequence_cnt;
}
