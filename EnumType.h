/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ENUM_TYPE_H_

#define ENUM_TYPE_H_

#include "ConstantExpression.h"
#include "Reference.h"
#include "Scope.h"

#include <vector>

namespace android {

struct EnumValue;
struct BitFieldType;

struct EnumType : public Scope {
    EnumType(const char* localName, const Location& location, const Reference<Type>& storageType,
             Scope* parent);

    const Type *storageType() const;
    const std::vector<EnumValue *> &values() const;
    void addValue(EnumValue *value);

    LocalIdentifier *lookupIdentifier(const std::string &name) const override;

    bool isElidableType() const override;
    const ScalarType *resolveToScalarType() const override;

    std::string typeName() const override;
    bool isEnum() const override;
    bool canCheckEquality() const override;

    std::string getCppType(StorageMode mode,
                           bool specifyNamespaces) const override;

    std::string getJavaType(bool forInitializer) const override;

    std::string getJavaSuffix() const override;

    std::string getJavaWrapperType() const override;

    std::string getVtsType() const override;

    // Return the type that corresponds to bitfield<T>.
    const BitFieldType* getBitfieldType() const;

    std::vector<const Reference<Type>*> getReferences() const override;
    std::vector<const ConstantExpression*> getConstantExpressions() const override;

    status_t resolveInheritance() override;
    status_t validate() const override;
    status_t validateUniqueNames() const;

    void emitReaderWriter(
            Formatter &out,
            const std::string &name,
            const std::string &parcelObj,
            bool parcelObjIsPointer,
            bool isReader,
            ErrorMode mode) const override;

    void emitJavaFieldReaderWriter(
            Formatter &out,
            size_t depth,
            const std::string &parcelName,
            const std::string &blobName,
            const std::string &fieldName,
            const std::string &offset,
            bool isReader) const override;

    status_t emitTypeDeclarations(Formatter &out) const override;
    void emitTypeForwardDeclaration(Formatter& out) const override;
    status_t emitGlobalTypeDeclarations(Formatter &out) const override;
    status_t emitTypeDefinitions(Formatter& out, const std::string& prefix) const override;

    status_t emitJavaTypeDeclarations(
            Formatter &out, bool atTopLevel) const override;

    status_t emitVtsTypeDeclarations(Formatter &out) const override;
    status_t emitVtsAttributeType(Formatter &out) const override;

    void emitJavaDump(
            Formatter &out,
            const std::string &streamName,
            const std::string &name) const override;

    void getAlignmentAndSize(size_t *align, size_t *size) const override;

    void appendToExportedTypesVector(
            std::vector<const Type *> *exportedTypes) const override;

    status_t emitExportedHeader(Formatter &out, bool forJava) const override;

   private:
    std::vector<const EnumType*> typeChain() const;
    std::vector<const EnumType*> superTypeChain() const;

    const Annotation *findExportAnnotation() const;

    void emitEnumBitwiseOperator(
            Formatter &out,
            bool lhsIsEnum,
            bool rhsIsEnum,
            const std::string &op) const;

    void emitBitFieldBitwiseAssignmentOperator(
            Formatter &out,
            const std::string &op) const;

    std::vector<EnumValue *> mValues;
    Reference<Type> mStorageType;
    // TODO(b/64272670): Dot not store BitFieldType as it is not owned.
    // It is kept here to avoid const-cast (BitFieldType owns non-const EnumType).
    BitFieldType* const mBitfieldType;

    DISALLOW_COPY_AND_ASSIGN(EnumType);
};

struct EnumValue : public LocalIdentifier {
    EnumValue(const char* name, ConstantExpression* value, const Location& location);

    std::string name() const;
    std::string value(ScalarType::Kind castKind) const;
    std::string cppValue(ScalarType::Kind castKind) const;
    std::string javaValue(ScalarType::Kind castKind) const;
    std::string comment() const;
    void autofill(const EnumType* prevType, EnumValue* prevValue, const ScalarType* type);
    ConstantExpression* constExpr() const override;

    bool isAutoFill() const;
    bool isEnumValue() const override;

    const Location& location() const;

   private:
    std::string mName;
    ConstantExpression* mValue;
    const Location mLocation;
    bool mIsAutoFill;

    DISALLOW_COPY_AND_ASSIGN(EnumValue);
};

struct BitFieldType : public TemplatedType {
    BitFieldType(Scope* parent);

    std::string templatedTypeName() const override;

    bool isBitField() const override;

    bool isCompatibleElementType(const Type* elementType) const override;

    bool isElidableType() const override;

    bool canCheckEquality() const override;

    const ScalarType *resolveToScalarType() const override;

    std::string getCppType(StorageMode mode,
                           bool specifyNamespaces) const override;

    std::string getJavaType(bool forInitializer) const override;

    std::string getJavaSuffix() const override;

    std::string getJavaWrapperType() const override;

    std::string getVtsType() const override;

    const EnumType* getEnumType() const;

    status_t emitVtsAttributeType(Formatter &out) const override;

    void getAlignmentAndSize(size_t *align, size_t *size) const override;

    void emitReaderWriter(
        Formatter &out,
        const std::string &name,
        const std::string &parcelObj,
        bool parcelObjIsPointer,
        bool isReader,
        ErrorMode mode) const override;

    void emitDump(
            Formatter &out,
            const std::string &streamName,
            const std::string &name) const override;

    void emitJavaDump(
            Formatter &out,
            const std::string &streamName,
            const std::string &name) const override;

    void emitJavaFieldReaderWriter(
        Formatter &out,
        size_t depth,
        const std::string &parcelName,
        const std::string &blobName,
        const std::string &fieldName,
        const std::string &offset,
        bool isReader) const override;
};

}  // namespace android

#endif  // ENUM_TYPE_H_

