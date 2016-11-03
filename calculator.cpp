#include <QtWidgets>

#include <cmath>

#include "button.h"
#include "calculator.h"

Calculator::Calculator(QWidget *parent)
    : QWidget(parent)
{

    display = new QLineEdit("0");
    display->setReadOnly(true);
    display->setAlignment(Qt::AlignRight);
    display->setMaxLength(15);

    QFont font = display->font();
    font.setPointSize(font.pointSize() + 8);
    display->setFont(font);

    // Create digit buttons
    for (int i = 0; i < NumDigitButtons; ++i) {
        digitButtons[i] = createButton(QString::number(i), SLOT(digitClicked()));
    }

    // Create buttons for interaction with the calculator
    Button *pointButton = createButton(tr("."), SLOT(pointClicked()));
    Button *changeSignButton = createButton(tr("\302\261"), SLOT(changeSignClicked()));

    Button *backspaceButton = createButton(tr("Backspace"), SLOT(backspaceClicked()));
    Button *clearAllButton = createButton(tr("Clear"), SLOT(clearAll()));

    Button *divisionButton = createButton(tr("\303\267"), SLOT(binaryOperatorClicked()));
    Button *timesButton = createButton(tr("\303\227"), SLOT(binaryOperatorClicked()));
    Button *minusButton = createButton(tr("-"), SLOT(binaryOperatorClicked()));
    Button *plusButton = createButton(tr("+"), SLOT(binaryOperatorClicked()));

    Button *squareRootButton = createButton(tr("Sqrt"), SLOT(unaryOperatorClicked()));
    Button *reciprocalButton = createButton(tr("1/x"), SLOT(unaryOperatorClicked()));
    Button *equalButton = createButton(tr("Enter"), SLOT(equalClicked()));

    // Create a grid layout and add the buttons to the interface
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    setWindowTitle(tr("Calculator"));

    for (int i = 1; i < NumDigitButtons; ++i) {
        int row = ((9 - i) / 3) + 2;
        int column = ((i - 1) % 3) + 1;
        mainLayout->addWidget(digitButtons[i], row, column);
    }

    mainLayout->addWidget(digitButtons[0], 5, 1, 1, 2);

    mainLayout->addWidget(display, 0, 0, 1, 6);
    mainLayout->addWidget(backspaceButton, 1, 1, 1, 2);
    mainLayout->addWidget(clearAllButton, 1, 3, 1, 2);
    mainLayout->addWidget(pointButton, 5, 3);
    mainLayout->addWidget(divisionButton, 2, 4);
    mainLayout->addWidget(timesButton, 3, 4);
    mainLayout->addWidget(minusButton, 4, 4);
    mainLayout->addWidget(plusButton, 5, 4);
    mainLayout->addWidget(squareRootButton, 2, 5);
    mainLayout->addWidget(reciprocalButton, 3, 5);
    mainLayout->addWidget(equalButton, 4, 5, 2, 1);
    mainLayout->addWidget(changeSignButton, 1, 5);
    setLayout(mainLayout);
}

// Function that updates the display when a digit is clicked
void Calculator::digitClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    double digitValue = clickedButton->text().toInt();

    operatorClicked = false;

    // If a number was previosly saved to the stack, start a new digit
    if (waitingForOperand)
    {
        display->setText("0");
        waitingForOperand = false;
    }

    if (display->text() == "0")
    {
        display->setText(QString::number(digitValue)); // if currently zero, replace with new input
    }
    else
    {
        display->setText(display->text() + QString::number(digitValue)); // append to the digit
    }
    return;
}

// Function that uses the displayed value, calculates the square root
// and displays the result
void Calculator::unaryOperatorClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    QString clickedOperator = clickedButton->text();
    double operand = display->text().toDouble();
    double result = 0.0;

    if (clickedOperator == tr("Sqrt")) {
        if (operand < 0.0) {
            abortOperation();
            return;
        }
        result = std::sqrt(operand);
    } else if (clickedOperator == tr("x\302\262")) {
        result = std::pow(operand, 2.0);
    } else if (clickedOperator == tr("1/x")) {
        if (operand == 0.0) {
            abortOperation();
            return;
        }
        result = 1.0 / operand;
    }
    display->setText(QString::number(result));
}

// Takes the top two elements from the stack and adds/subtracts them
void Calculator::binaryOperatorClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    QString clickedOperator = clickedButton->text();
    operatorClicked = true;

    pendingOperator = clickedOperator;
}

void Calculator::equalClicked()
{

    double input = display->text().toDouble();
   // double operand = display->text().toDouble();

    if (operatorClicked)
    {
        if (!calculate(pendingOperator))
        {
            abortOperation();
            return;
        }
        else
        {
            display->setText(QString::number(result.top()));
        }
        operatorClicked = false;
    }
    else
    {
        result.push(input);
        waitingForOperand = true;
    }
}

// Add a decimal point
void Calculator::pointClicked()
{
    if (!display->text().contains('.'))
        display->setText(display->text() + tr("."));
}

// Function that changes the sign of the digits
void Calculator::changeSignClicked()
{
    QString text = display->text();
    double value = text.toDouble();

    if (value > 0.0) {
        text.prepend(tr("-"));
    } else if (value < 0.0) {
        text.remove(0, 1);
    }
    display->setText(text);
}

// Function that removes the last digit clicked
void Calculator::backspaceClicked()
{

    QString text = display->text();
    text.chop(1);
    if (text.isEmpty()) {
        text = "0";
    }
    display->setText(text);
}

// Function that sets all variables to a default
void Calculator::clearAll()
{
    pendingOperator.clear();
    display->setText("0");
    waitingForOperand = false;
    operatorClicked = false;

    for (int i = 0; i < result.size(); i++)
    {
        result.pop();
    }
}

// Function used to create buttons for the interface
Button *Calculator::createButton(const QString &text, const char *member)
{
    Button *button = new Button(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}

// Function that displays an error
void Calculator::abortOperation()
{
    clearAll();
    display->setText(tr("Invalid Input"));
}

bool Calculator::calculate(const QString &pendingOperator)
{
    double first_operand, second_operand;

    if (result.size() < 2) {
        return false; // not enough arguments
    }
    else
    {
        first_operand = result.top();
        result.pop();
        second_operand = result.top();
        result.pop();
    }

    if (pendingOperator == tr("+"))
    {
        result.push(first_operand + second_operand);
    }
    else if (pendingOperator == tr("-"))
    {
        result.push(second_operand - first_operand);
    }
    else if (pendingOperator == tr("\303\227"))
    {
        result.push(first_operand * second_operand);
    }
    else if (pendingOperator == tr("\303\267"))
    {
        if (second_operand == 0.0)
        {
            return false; // can't divide by zero
        }
        result.push(second_operand / first_operand);
    }

    display->setText(QString::number(result.top()));
    return true;
}
