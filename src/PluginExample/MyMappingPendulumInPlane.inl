/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#pragma once

#include <PluginExample/MyMappingPendulumInPlane.h>

#include <sofa/simulation/Simulation.h>
#include <sofa/core/visual/VisualParams.h>
#include <sofa/core/MechanicalParams.h>
#include <iostream>

using std::cerr;
using std::endl;


namespace pluginexample
{

using helper::ReadAccessor;
using helper::WriteAccessor;
using type::Vec;
using type::Vec3;


template <class In, class Out>
MyMappingPendulumInPlane<In, Out>::MyMappingPendulumInPlane():
    Inherit(),
    d_length(initData(&d_length, "lengths", "distances from the fixed point to the end of the pendulum"))
{
}


template <class In, class Out>
MyMappingPendulumInPlane<In, Out>::~MyMappingPendulumInPlane()
{
}

template <class In, class Out>
void MyMappingPendulumInPlane<In, Out>::init()
{
    ReadAccessor<Data<VecOutCoord> > out (*this->toModel->read(core::vec_id::read_access::position));
    WriteAccessor<Data<VecInCoord> > in (*this->fromModel->write(core::vec_id::write_access::position));
    WriteAccessor<Data<vector<OutReal> > > distances (d_length);
    if (distances.size() != out.size()) // values not read from file
    {
        in.resize(out.size());
        distances.resize(out.size());
        gap.resize(out.size());
        for(unsigned i=0; i<out.size(); i++)
        {
            typename Out::Real x, y,z;
            Out::get(x, y,z, out[i]);
            in[i][0] = (InReal) atan2(y, x);
            distances[i] = sqrt(x*x+y*y);
        }
    }
}

template <class In, class Out>
void MyMappingPendulumInPlane<In, Out>::draw(const core::visual::VisualParams* vparams)
{
    if (!vparams->displayFlags().getShowMappings()) return;

    ReadAccessor<Data<VecOutCoord> > out (*this->toModel->read(core::vec_id::read_access::position));
    std::vector< Vec3 > points(out.size());

    for (unsigned int i=0; i<out.size(); i++)
    {
        points[i] = Out::getCPos(out[i]);
    }
    vparams->drawTool()->drawPoints (points, 7, type::RGBAColor::yellow());

    points.resize(2*out.size());
    for (unsigned int i=0; i<out.size(); i++)
    {
        points[2*i] =   Vec3(0, 0, 0);
        points[2*i+1] = Out::getCPos(out[i]);
    }
    vparams->drawTool()->drawLines (points, 1, type::RGBAColor::green());


}

template <class In, class Out>
void MyMappingPendulumInPlane<In, Out>::apply(const core::MechanicalParams* mparams,
                                             OutDataVecCoord& out,
                                             const InDataVecCoord& in)
{
    SOFA_UNUSED(mparams);

    VecOutCoord& childPos = *out.beginEdit();
    const VecInCoord& parentPos = in.getValue();

    ReadAccessor<Data<vector<OutReal> > > distances (d_length);
    for(unsigned i=0; i<childPos.size(); i++)
    {
        gap[i] = Vec2(distances[i] * cos(parentPos[i][0]),
                      distances[i] * sin(parentPos[i][0]));
        childPos[i][0] = gap[i][0];
        childPos[i][1] = gap[i][1];
    }

    out.endEdit();
}

template <class In, class Out>
void MyMappingPendulumInPlane<In, Out>::applyJ(const core::MechanicalParams* mparams,
                                              OutDataVecDeriv& out,
                                              const InDataVecDeriv& in)
{
    SOFA_UNUSED(mparams);

    VecOutDeriv& childVel = *out.beginEdit();
    const VecInDeriv& parentVel = in.getValue();

    for(unsigned i=0; i<childVel.size(); i++)
    {
        // velocity is orthogonal to the radius and proportional with the angular velocity
        Out::set(childVel[i],
                 (OutReal)(-gap[i][1] * parentVel[i][0]),
                 (OutReal)(+gap[i][0] * parentVel[i][0]),
                 (OutReal)0);
    }

    out.endEdit();
}

template <class In, class Out>
void MyMappingPendulumInPlane<In, Out>::applyJT(const core::MechanicalParams* mparams,
                                               InDataVecDeriv& out,
                                               const OutDataVecDeriv& in)
{
    SOFA_UNUSED(mparams);

    VecInDeriv& parentForce = *out.beginEdit();
    const VecOutDeriv& childForce = in.getValue();

    for(unsigned i=0; i<parentForce.size(); i++)
    {
        // convert force to torque
        parentForce[i][0] += -gap[i][1] * childForce[i][0] + gap[i][0] * childForce[i][1] ;
    }

    out.endEdit();
}

template <class In, class Out>
void MyMappingPendulumInPlane<In, Out>::applyJT(const core::ConstraintParams* cparams,
                                               InDataMatrixDeriv& out,
                                               const OutDataMatrixDeriv& in)
{
    SOFA_UNUSED(cparams);

    MatrixInDeriv& parentJacobians = *out.beginEdit();
    const MatrixOutDeriv& childJacobians = in.getValue();

    for (typename Out::MatrixDeriv::RowConstIterator childJacobian = childJacobians.begin(); childJacobian != childJacobians.end(); ++childJacobian)
    {
        typename In::MatrixDeriv::RowIterator parentJacobian = parentJacobians.writeLine(childJacobian.index());

        for (typename Out::MatrixDeriv::ColConstIterator childParticle = childJacobian.begin(); childParticle != childJacobian.end(); ++childParticle)
        {
            unsigned int childIndex = childParticle.index();
            const OutDeriv& childJacobianVec = childParticle.val();

            parentJacobian.addCol(childIndex, InDeriv(-gap[childIndex][1] * childJacobianVec[0] + gap[childIndex][0] * childJacobianVec[1]));
        }
    }

    out.endEdit();
}

template <class In, class Out>
void MyMappingPendulumInPlane<In, Out>::applyDJT(const core::MechanicalParams* mparams,
                                                core::MultiVecDerivId parentForceChangeId,
                                                core::ConstMultiVecDerivId)
{
    ReadAccessor<Data<VecOutDeriv> > childForce (*mparams->readF(this->toModel.get()));
    WriteAccessor<Data<VecInDeriv> > parentForce (*parentForceChangeId[this->fromModel.get()].write());
    ReadAccessor<Data<VecInDeriv> > parentDx (*mparams->readDx(this->fromModel.get()));
    InReal kfactor = (InReal)mparams->kFactor();

    for(unsigned i=0; i<parentForce.size(); i++)
    {
        parentForce[i][0] -= (gap[i][0] * childForce[i][0] +  gap[i][1] * childForce[i][1]) * parentDx[i][0] * kfactor;
    }
}


}	// namespace pluginexample 
