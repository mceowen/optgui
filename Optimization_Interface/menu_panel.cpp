// TITLE:   Optimization_Interface/menu_panel.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "menu_panel.h"

#include <QHBoxLayout>
#include <QPainter>
#include <QtMath>

#include "view.h"

namespace interface {

MenuPanel::MenuPanel(QWidget *parent, Qt::WindowFlags flags)
    : QFrame(parent, flags) {
    this->initialize();
}

MenuPanel::~MenuPanel() {
    // delete menu controls
    for (MenuButton *button : *this->menu_buttons_) {
        delete button;
    }
    delete this->menu_buttons_;
    delete this->exec_button_;
    delete this->zoom_slider_;
    delete this->menu_->layout();

    // delete menu panel components
    delete this->menu_;
    delete this->close_button_;
    delete this->layout();
}

void MenuPanel::initialize() {
    // Set color
    this->setAutoFillBackground(true);
    QPalette palette = this->palette();
    QColor background = QWidget::palette().window().color();
    background.setAlpha(200);
    palette.setColor(QPalette::Base, background);
    this->setPalette(palette);

    // Set frame
    this->setFrameShape(QFrame::Panel);
    this->setFrameShadow(QFrame::Raised);

    // Set layout
    this->setLayout(new QHBoxLayout(this));
    this->layout()->setContentsMargins(1, 1, 1, 1);
    this->layout()->setSpacing(1);

    // Create menu close button
    this->close_button_ = new QToolButton(this);
    this->close_button_->setArrowType(Qt::RightArrow);
    this->close_button_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->close_button_->setFixedSize(10, 40);
    this->layout()->addWidget(this->close_button_);
    this->layout()->setAlignment(this->close_button_, Qt::AlignLeft);

    // Create menu
    this->menu_ = new QWidget(this);
    this->menu_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->menu_->setLayout(new QVBoxLayout(this->menu_));
    this->menu_->layout()->setContentsMargins(0, 10, 0, 10);
    this->menu_->layout()->setSpacing(0);
    this->layout()->addWidget(this->menu_);

    // Create menu buttons
    this->menu_buttons_ = new QVector<MenuButton*>();
    this->initializeEllipseButton();
    this->initializePolygonButton();
    this->initializePlaneButton();
    this->initializeWaypointButton();
    this->initializeEraserButton();
    this->initializeFlipButton();

    // Show menu buttons
    for (MenuButton *button : *this->menu_buttons_) {
        this->menu_->layout()->addWidget(button);
        this->menu_->layout()->setAlignment(button,
                                            Qt::AlignTop|Qt::AlignCenter);
    }

    // Create execute button
    this->initializeExecButton();

    // Create zoom slider
    this->initializeZoomSlider();
}

void MenuPanel::initializeEllipseButton() {
    MenuButton *ellipse_button = new MenuButton(ELLIPSE, this->menu_);
    QPixmap pix(50, 50);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(Qt::black);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.setBrush(Qt::gray);
    painter.drawEllipse(1, 1, 48, 48);
    ellipse_button->setPixmap(pix);
    ellipse_button->setToolTip(tr("Add ellipse constraint"));
    this->menu_buttons_->append(ellipse_button);
}

void MenuPanel::initializePolygonButton() {
    MenuButton *polygon_button = new MenuButton(POLYGON, this->menu_);
    QPixmap pix(50, 50);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(Qt::black);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.setBrush(Qt::gray);

    QPolygonF poly;
    poly << QPointF(25, 1);
    poly << QPointF(49, 21);
    poly << QPointF(40, 49);
    poly << QPointF(10, 49);
    poly << QPointF(1, 21);
    painter.drawPolygon(poly);

    polygon_button->setPixmap(pix);
    polygon_button->setToolTip(tr("Add polygon constraint"));
    this->menu_buttons_->append(polygon_button);
}

void MenuPanel::initializePlaneButton() {
    MenuButton *plane_button = new MenuButton(PLANE, this->menu_);
    QPixmap pix(50, 50);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(Qt::black);
    pen.setWidth(3);
    painter.setPen(pen);
    painter.setBrush(Qt::gray);
    painter.drawLine(QPointF(10, 40), QPointF(40, 10));

    plane_button->setPixmap(pix);
    plane_button->setToolTip(tr("Add plane constraint"));
    this->menu_buttons_->append(plane_button);
}

void MenuPanel::initializeEraserButton() {
    MenuButton *eraser_button = new MenuButton(ERASER, this->menu_);
    QPixmap pix(":/images/erasericon.png");
    pix = pix.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    eraser_button->setPixmap(pix);
    eraser_button->setToolTip(tr("Delete item"));
    this->menu_buttons_->append(eraser_button);
}

void MenuPanel::initializeWaypointButton() {
    MenuButton *waypoint_button = new MenuButton(WAYPOINT, this->menu_);
    QPixmap pix(50, 50);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(Qt::black);
    pen.setWidth(1);
    painter.setPen(pen);
    painter.setBrush(Qt::red);

    painter.drawEllipse(QPointF(7, 7), 6, 6);
    painter.setBrush(Qt::white);
    painter.drawEllipse(QPointF(19, 19), 6, 6);
    painter.drawEllipse(QPointF(31, 31), 6, 6);
    painter.setBrush(Qt::green);
    painter.drawEllipse(QPointF(42, 42), 6, 6);

    waypoint_button->setPixmap(pix);
    waypoint_button->setToolTip(tr("Add waypoint"));
    this->menu_buttons_->append(waypoint_button);
}

void MenuPanel::initializeFlipButton() {
    MenuButton *flip_button = new MenuButton(FLIP, this->menu_);
    QPixmap pix(50, 50);
    pix.fill(Qt::transparent);
    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(Qt::black);
    pen.setWidth(3);
    painter.setPen(pen);
    painter.setBrush(Qt::gray);

    QLineF line(QPointF(10, 40), QPointF(40, 10));

    QPolygonF poly;
    poly << line.p1();
    poly << line.p2();
    poly << line.normalVector().translated(
                line.dx(), line.dy()).pointAt(10.0 / line.length());
    poly << line.normalVector().pointAt(10.0 / line.length());

    painter.setPen(Qt::NoPen);
    painter.drawPolygon(poly);
    painter.setPen(pen);
    painter.drawLine(line);

    flip_button->setPixmap(pix);
    flip_button->setToolTip(tr("Flip constraint directionity"));
    this->menu_buttons_->append(flip_button);
}

void MenuPanel::initializeExecButton() {
    this->exec_button_ = new QPushButton("Exec", this->menu_);
    this->exec_button_->setToolTip(tr("Execute optimization with constraints"));
    this->menu_->layout()->addWidget(this->exec_button_);
    this->menu_->layout()->setAlignment(this->exec_button_, Qt::AlignBottom);
}

void MenuPanel::initializeZoomSlider() {
    this->zoom_slider_ = new QSlider(Qt::Horizontal, this->menu_);
    this->zoom_slider_->setSizePolicy(QSizePolicy::Expanding,
                                      QSizePolicy::Minimum);
    this->zoom_slider_->setTickInterval(1);
    this->zoom_slider_->setTickPosition(QSlider::TicksAbove);
    this->zoom_slider_->setMinimum(2);
    this->zoom_slider_->setMaximum(8);
    this->zoom_slider_->setValue(5);
    this->zoom_slider_->setToolTip(tr("Set zoom level"));
    this->menu_->layout()->addWidget(this->zoom_slider_);
    this->menu_->layout()->setAlignment(this->zoom_slider_, Qt::AlignBottom);
}

}  // namespace interface
