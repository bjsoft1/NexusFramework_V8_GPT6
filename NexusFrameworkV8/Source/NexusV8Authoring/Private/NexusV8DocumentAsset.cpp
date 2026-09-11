#include "NexusV8DocumentAsset.h"
bool UNexusV8DocumentAsset::Read(nexus::Document& Out, FString& Error) const
{
    nexus::Bytes Bytes;
    if (!DocumentBytes.IsEmpty())
        Bytes.assign(DocumentBytes.GetData(), DocumentBytes.GetData()+DocumentBytes.Num());
    const auto Issues=nexus::decode(Bytes,Out);
    for(const auto& Issue:Issues) if(Issue.blocking) Error+=UTF8_TO_TCHAR((Issue.code+": "+Issue.message+"\n").c_str());
    return !nexus::hasErrors(Issues);
}
bool UNexusV8DocumentAsset::Write(const nexus::Document& In,FString& Error)
{
    const auto Issues=nexus::validate(In);
    for(const auto& Issue:Issues) if(Issue.blocking) Error+=UTF8_TO_TCHAR((Issue.code+": "+Issue.message+"\n").c_str());
    if(nexus::hasErrors(Issues)) return false;
    const auto Bytes=nexus::encode(In);
    DocumentBytes.Reset(static_cast<int32>(Bytes.size()));
    DocumentBytes.Append(Bytes.data(),static_cast<int32>(Bytes.size()));
    return true;
}
