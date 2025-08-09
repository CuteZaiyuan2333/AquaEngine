#include "AquaVisual/Math.h"
#include "AquaVisual/Math/Matrix.h"

namespace AquaVisual {

Matrix4 Transform::GetMatrix() const {
    // Create transformation matrix: T * R * S
    Matrix4 translation = Matrix4::Translate(position);
    Matrix4 rotationX = Matrix4::RotateX(rotation.x);
    Matrix4 rotationY = Matrix4::RotateY(rotation.y);
    Matrix4 rotationZ = Matrix4::RotateZ(rotation.z);
    Matrix4 scaling = Matrix4::Scale(scale);
    
    // Combine transformations: Translation * RotationZ * RotationY * RotationX * Scale
    return translation * rotationZ * rotationY * rotationX * scaling;
}

} // namespace AquaVisual