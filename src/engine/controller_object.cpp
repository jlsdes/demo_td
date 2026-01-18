#include "controller_object.hpp"
#include "utils/log.hpp"


ControllerObject::ControllerObject( ModelObject * model ) : ManagedObject {}, m_model { model } {
    // ControllerObjects without a ModelObject are probably useless... so (for now) report a warning
    if ( not m_model )
        Log::warning( "Created a ControllerObject without a ModelObject attached." );
}
