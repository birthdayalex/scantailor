/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C)  Joseph Artsimovich <joseph.artsimovich@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "DistortionModel.h"
#include "NumericTraits.h"
#include <QRectF>
#include <QPointF>
#include <QTransform>
#include <QString>
#include <QDomDocument>
#include <QDomElement>
#include <boost/foreach.hpp>

namespace output
{

DistortionModel::DistortionModel()
{
}

DistortionModel::DistortionModel(QDomElement const& el)
:	m_topCurve(el.namedItem("top-curve").toElement()),
	m_bottomCurve(el.namedItem("bottom-curve").toElement())
{
}

QDomElement
DistortionModel::toXml(QDomDocument& doc, QString const& name) const
{
	if (!isValid()) {
		return QDomElement();
	}

	QDomElement el(doc.createElement(name));
	el.appendChild(m_topCurve.toXml(doc, "top-curve"));
	el.appendChild(m_bottomCurve.toXml(doc, "bottom-curve"));
	return el;
}

bool
DistortionModel::isValid() const
{
	return m_topCurve.isValid() && m_bottomCurve.isValid();
}

bool
DistortionModel::matches(DistortionModel const& other) const
{
	bool const this_valid = isValid();
	bool const other_valid = other.isValid();
	if (!this_valid && !other_valid) {
		return true;
	} else if (this_valid != other_valid) {
		return false;
	}

	if (!m_topCurve.matches(other.m_topCurve)) {
		return false;
	} else if (!m_bottomCurve.matches(other.m_bottomCurve)) {
		return false;
	}

	return true;
}

QRectF
DistortionModel::boundingBox(QTransform const& transform) const
{
	double top = NumericTraits<double>::max();
	double left = top;
	double bottom = NumericTraits<double>::min();
	double right = bottom;

	BOOST_FOREACH(QPointF pt, m_topCurve.polyline()) {
		pt = transform.map(pt);
		left = std::min<double>(left, pt.x());
		right = std::max<double>(right, pt.x());
		top = std::min<double>(top, pt.y());
		bottom = std::max<double>(bottom, pt.y());
	}

	BOOST_FOREACH(QPointF pt, m_bottomCurve.polyline()) {
		pt = transform.map(pt);
		left = std::min<double>(left, pt.x());
		right = std::max<double>(right, pt.x());
		top = std::min<double>(top, pt.y());
		bottom = std::max<double>(bottom, pt.y());
	}

	if (top > bottom || left > right) {
		return QRectF();
	} else {
		return QRectF(left, top, right - left, bottom - top);
	}
}

} // namespace output
