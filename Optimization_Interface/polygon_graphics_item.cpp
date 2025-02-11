// TITLE:   Optimization_Interface/polygon_graphics_item.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "polygon_graphics_item.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLineF>

#include "globals.h"

namespace interface {

PolygonGraphicsItem::PolygonGraphicsItem(PolygonModelItem *model,
                                         QGraphicsItem *parent)
    : QGraphicsItem(parent) {
    // Set model
    this->model_ = model;
    this->initialize();
}

void PolygonGraphicsItem::initialize() {
    // Set pen
    QColor fill = Qt::gray;
    fill.setAlpha(200);
    this->brush_ = QBrush(fill);

    // Set brush
    this->pen_ = QPen(Qt::black);
    this->pen_.setWidth(3);

    // Set flags
    this->setFlags(QGraphicsItem::ItemIsMovable |
                   QGraphicsItem::ItemIsSelectable |
                   QGraphicsItem::ItemSendsGeometryChanges);

    // Set resize handles
    this->resize_handles_ = new QVector<PolygonResizeHandle *>();
    for (QPointF *point : *this->model_->points_) {
        PolygonResizeHandle *handle = new PolygonResizeHandle(point, this);
        this->resize_handles_->append(handle);
        handle->hide();
    }
}

PolygonGraphicsItem::~PolygonGraphicsItem() {
    // Delete resize handles
    for (PolygonResizeHandle *handle : *this->resize_handles_) {
        delete handle;
    }
    delete this->resize_handles_;
}

QRectF PolygonGraphicsItem::boundingRect() const {
    return this->shape().boundingRect();
}

void PolygonGraphicsItem::paint(QPainter *painter,
                                const QStyleOptionGraphicsItem *option,
                                QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Change color based on convexity
    QColor fill;
    if (this->isConvex()) {
        fill = Qt::gray;
    } else {
        fill = Qt::red;
    }
    fill.setAlpha(200);
    this->brush_ = QBrush(fill);

    // Show handles if selected
    if (this->isSelected()) {
        for (PolygonResizeHandle *handle : *this->resize_handles_) {
            handle->updatePos();
            handle->show();
        }
        this->pen_.setWidth(3);
    } else {
        for (PolygonResizeHandle *handle : *this->resize_handles_) {
            handle->hide();
        }
        this->pen_.setWidth(1);
    }

    painter->setPen(this->pen_);
    painter->setBrush(this->brush_);

    // Fill shading
    painter->fillPath(this->shape(), this->brush_);

    // Draw outline
    for (qint32 i = 1; i < this->model_->points_->length(); i++) {
        QLineF line(mapFromScene(*this->model_->points_->at(i-1)),
                    mapFromScene(*this->model_->points_->at(i)));
        painter->drawLine(line);
    }
    QLineF line(mapFromScene(*this->model_->points_->last()),
                mapFromScene(*this->model_->points_->first()));
    painter->drawLine(line);

    // Label with port
    if (!this->model_->points_->isEmpty() && this->model_->port_ != 0) {
        QPointF text_pos(this->mapFromScene(*this->model_->points_->first()));
        painter->drawText(QRectF(text_pos.x(), text_pos.y(), 50, 15),
                          QString::number(this->model_->port_));
    }
}

int PolygonGraphicsItem::type() const {
    return POLYGON_GRAPHIC;
}

QPainterPath PolygonGraphicsItem::shape() const {
    QPainterPath path;

    // Define exterior shadings
    for (qint32 i = 1; i < this->model_->points_->length(); i++) {
        QLineF line(mapFromScene(*this->model_->points_->at(i-1)),
                    mapFromScene(*this->model_->points_->at(i)));
        // Flip shading if direction is reversed
        if (this->model_->direction_) {
            line = QLineF(line.p2(), line.p1());
        }
        QPolygonF poly;
        poly << line.p1();
        poly << line.p2();
        poly << line.normalVector().translated(
                    line.dx(),
                    line.dy()).pointAt(POLYGON_BORDER /line.length());
        poly << line.normalVector().pointAt(POLYGON_BORDER / line.length());
        path.addPolygon(poly);
    }
    QLineF line(mapFromScene(*this->model_->points_->last()),
                mapFromScene(*this->model_->points_->first()));
    // Flip shading if direction is reversed
    if (this->model_->direction_) {
        line = QLineF(line.p2(), line.p1());
    }
    QPolygonF poly;
    poly << line.p1();
    poly << line.p2();
    poly << line.normalVector().translated(
                line.dx(), line.dy()).pointAt(POLYGON_BORDER / line.length());
    poly << line.normalVector().pointAt(POLYGON_BORDER / line.length());
    path.addPolygon(poly);

    // Return shape
    return path;
}

void PolygonGraphicsItem::expandScene() {
    if (scene()) {
        // expand scene if item goes out of bounds
        QRectF newRect = this->sceneBoundingRect();
        QRectF rect = this->scene()->sceneRect();
        if (!rect.contains(newRect)) {
            this->scene()->setSceneRect(
                        this->scene()->sceneRect().united(newRect));
            if (!this->scene()->views().isEmpty()) {
                this->scene()->views().first()->setSceneRect(
                            this->scene()->sceneRect());
            }
        }
        this->scene()->update();
    }
}

void PolygonGraphicsItem::flipDirection() {
    this->model_->direction_ = !this->model_->direction_;
    this->expandScene();
}

QVariant PolygonGraphicsItem::itemChange(GraphicsItemChange change,
                                         const QVariant &value) {
    if (change == ItemPositionChange && scene()) {
        // value is the new position.
        QPointF newPos = value.toPointF();

        // update model
        QPointF diff = newPos - this->scenePos();
        for (PolygonResizeHandle *handle : *this->resize_handles_) {
            handle->updateModel(diff);
        }

        // check to expand the scene
        this->expandScene();
    }
    return QGraphicsItem::itemChange(change, value);
}

bool PolygonGraphicsItem::isConvex() const {
    quint32 n = this->model_->points_->size();
    if (n < 4) {
        return true;
    }

    bool sign = false;
    QVector<QPointF *> *points = this->model_->points_;

    for (quint32 i = 0; i < n; i++) {
        qreal dx1 = points->at((i + 2) % n)->x() - points->at((i + 1) % n)->x();
        qreal dy1 = points->at((i + 2) % n)->y() - points->at((i + 1) % n)->y();
        qreal dx2 = points->at(i)->x() - points->at((i + 1) % n)->x();
        qreal dy2 = points->at(i)->y() - points->at((i + 1) % n)->y();
        qreal z_cross_product = (dx1 * dy2) - (dy1 * dx2);

        if (i == 0) {
            sign = z_cross_product > 0;
        } else if (sign != (z_cross_product > 0)) {
            return false;
        }
    }
    return true;
}

}  // namespace interface
