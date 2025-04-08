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
#include <PluginExample/config.h>

#include <sofa/core/ObjectFactory.h>
using sofa::core::ObjectFactory;
#include <sofa/helper/system/PluginManager.h>

namespace sofa::component::behaviormodel
{
    extern void registerMyBehaviorModel(sofa::core::ObjectFactory* factory);
}
namespace sofa::component::mapping
{
    extern void registerMyMappingPendulumInPlane(sofa::core::ObjectFactory* factory);
}
namespace sofa::component::projectiveconstraintset
{
    extern void registerMyProjectiveConstraintSet(sofa::core::ObjectFactory* factory);
}
namespace sofa::component::visual
{
    extern void registerMyVisualModel(sofa::core::ObjectFactory* factory);
}

extern "C" {
    SOFA_PLUGINEXAMPLE_API void initExternalModule();
    SOFA_PLUGINEXAMPLE_API const char* getModuleName();
    SOFA_PLUGINEXAMPLE_API const char* getModuleVersion();
    SOFA_PLUGINEXAMPLE_API const char* getModuleLicense();
    SOFA_PLUGINEXAMPLE_API const char* getModuleDescription();
    SOFA_PLUGINEXAMPLE_API const char* getModuleComponentList();
    SOFA_PLUGINEXAMPLE_API void registerObjects(sofa::core::ObjectFactory* factory);
}

void initExternalModule()
{
    static bool first = true;
    if (first)
    {
        first = false;

        // make sure that this plugin is registered into the PluginManager
        sofa::helper::system::PluginManager::getInstance().registerPlugin(sofa_tostring(SOFA_TARGET));
    }
}

const char* getModuleName()
{
    return sofa_tostring(SOFA_TARGET);
}

const char* getModuleVersion()
{
    return sofa_tostring(PLUGINEXAMPLE_VERSION);
}

const char* getModuleLicense()
{
    return "LGPL";
}

const char* getModuleDescription()
{
    return "Simple example of a Sofa plugin.";
}

const char* getModuleComponentList()
{
    /// string containing the names of the classes provided by the plugin
    static std::string classes = ObjectFactory::getInstance()->listClassesFromTarget(sofa_tostring(SOFA_TARGET));
    return classes.c_str();
}

void registerObjects(sofa::core::ObjectFactory* factory)
{
    sofa::component::behaviormodel::registerMyBehaviorModel(factory);
    sofa::component::mapping::registerMyMappingPendulumInPlane(factory);
    sofa::component::projectiveconstraintset::registerMyProjectiveConstraintSet(factory);
    sofa::component::visual::registerMyVisualModel(factory);
}
