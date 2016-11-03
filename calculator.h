#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QWidget>
#include <stack>

class QLineEdit;

class Button;

class Calculator : public QWidget
{
    Q_OBJECT

public:
    Calculator(QWidget *parent = 0);

private slots:
    void digitClicked();
    void unaryOperatorClicked();
    void binaryOperatorClicked();
    void equalClicked();
    void pointClicked();
    void changeSignClicked();
    void backspaceClicked();
    void clearAll();

private:
    Button *createButton(const QString &text, const char *member);
    void abortOperation();
    bool calculate(const QString &pendingOperator);

    QString pendingOperator;

    std::stack<double> result;
    std::stack<double> factor;

    bool waitingForOperand;
    bool operatorClicked;


    QLineEdit *display;

    enum { NumDigitButtons = 10 };
    Button *digitButtons[NumDigitButtons];
};

#endif
