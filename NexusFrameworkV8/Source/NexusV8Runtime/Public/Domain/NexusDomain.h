#pragma once
// The same dependency-free C++17 domain is compiled by Unreal and the portable tests.
#include <array>
#include <cstdint>
#include <functional>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>
#ifndef NEXUSV8RUNTIME_API
#define NEXUSV8RUNTIME_API
#endif
namespace nexus {
    using Id = std::string;
    // canonical lower-case 32-digit GUID, never an array index
    constexpr std::uint32_t SchemaVersion = 1;
    constexpr std::uint32_t Car = 1, Bus = 2, Emergency = 4, Service = 8;
    struct Issue {
        std::string code;
        Id entity;
        std::string message;
        bool blocking = true;
    };
    using Issues = std::vector<Issue>;
    NEXUSV8RUNTIME_API bool hasErrors(const Issues&);
    NEXUSV8RUNTIME_API bool validId(const Id&);
    NEXUSV8RUNTIME_API Id fixtureId(std::uint64_t value);
    NEXUSV8RUNTIME_API std::string fingerprint(const std::string& bytes);
    struct Vec3 {
        double x=0, y=0, z=0;
        Vec3 operator+(Vec3 b) const {
            return {
                x+b.x,y+b.y,z+b.z
            };
        }
        Vec3 operator-(Vec3 b) const {
            return {
                x-b.x,y-b.y,z-b.z
            };
        }
        Vec3 operator*(double s) const {
            return {
                x*s,y*s,z*s
            };
        }
    };
    NEXUSV8RUNTIME_API double dot(Vec3 a, Vec3 b);
    NEXUSV8RUNTIME_API Vec3 cross(Vec3 a, Vec3 b);
    NEXUSV8RUNTIME_API double length(Vec3 a);
    NEXUSV8RUNTIME_API Vec3 unit(Vec3 a);
    NEXUSV8RUNTIME_API bool finite(Vec3 a);
    struct Quat {
        double x=0,y=0,z=0,w=1;
    };
    NEXUSV8RUNTIME_API Quat normalized(Quat q);
    NEXUSV8RUNTIME_API Quat multiply(Quat a, Quat b);
    NEXUSV8RUNTIME_API Vec3 rotate(Quat q, Vec3 v);
    struct Pose {
        Vec3 position;
        Quat rotation;
    };
    NEXUSV8RUNTIME_API Pose compose(Pose frame, Pose local);
    NEXUSV8RUNTIME_API Pose relative(Pose frame, Pose world);
    enum class Kind : std::uint8_t {
        World, State, City, Highway, Point, Place
    };
    enum class Direction : std::uint8_t {
        Forward, Reverse
    };
    enum class DrivingSide : std::uint8_t {
        Right, Left
    };
    enum class Availability : std::uint8_t {
        Loaded, Unloaded, Missing, Deleted, Ambiguous
    };
    enum class Assignment : std::uint8_t {
        Inherit, Assigned, Disabled
    };
    enum class LinkKind : std::uint8_t {
        Continue, Merge, Branch
    };
    struct Payload {
        std::string type;
        std::uint32_t version=1;
        bool enabled=true;
        std::string bytes;
    };
    struct Entity {
        Id id, parent;
        Kind kind=Kind::World;
        std::string name;
        Id catalog, defaultProfile;
        DrivingSide drivingSide=DrivingSide::Right;
        std::string sourceKey;
        // opaque source key; NEVER authored geometry
        Availability availability=Availability::Loaded;
        std::string acceptedSourceFingerprint;
        Id anchorSegment;
        // for Point-relative Places, disambiguates branches
        bool anchorAtEnd=false;
        Pose local;
        // only legal for Place
        Payload payload;
    };
    struct Lane {
        Id id;
        Direction direction=Direction::Forward;
        std::uint32_t ordinal=0;
        // median outward, independently per travel direction
        double widthM=3.5, speedMps=13.8888888889;
        std::uint32_t access=Car|Bus|Emergency|Service;
    };
    struct Profile {
        Id id;
        std::string name;
        std::vector<Lane> lanes;
        double medianM=0.3, sidewalkLeftM=2, sidewalkRightM=2;
        double borderLeftM=0.25, borderRightM=0.25, sidewalkHeightM=0.15;
    };
    struct Section {
        double startM=0;
        Id profile;
    };
    struct LaneLink {
        Id id, fromLane, toLane;
        Direction direction=Direction::Forward;
        LinkKind kind=LinkKind::Continue;
        bool approved=false;
    };
    struct Adapter {
        Id id;
        double startM=0,endM=0;
        Id before, after;
        std::vector<LaneLink> links;
        double minimumTaperRatio=10;
        // explicit project rule, not a road-design standard
    };
    struct Segment {
        Id id, highway, startPoint, endPoint;
        std::string sourceKey, acceptedSourceFingerprint;
        Availability availability=Availability::Loaded;
        std::vector<Section> sections;
        std::vector<Adapter> adapters;
    };
    struct Port {
        std::string name;
        std::vector<double> forwardWidths, reverseWidths;
        double sidewalkLeftM=0,sidewalkRightM=0,borderLeftM=0,borderRightM=0;
    };
    struct MeshInterface {
        double metersPerUnit=0.01;
        std::string forwardAxis="X", upAxis="Z", pivot="StartCenter";
        double lengthM=10;
        bool openEnds=true, textureMarkings=true;
        std::vector<Port> ports;
    };
    struct MeshEntry {
        Assignment assignment=Assignment::Inherit;
        std::string assetPath;
        MeshInterface interface;
    };
    struct Catalog {
        Id id;
        std::string name;
        std::map<std::string,MeshEntry> entries;
    };
    struct SharedTopology {
        Id id, meshOwnerHighway;
        std::string type="Junction";
        std::vector<Id> approachPoints;
    };
    struct Document {
        std::uint32_t schema=SchemaVersion;
        Id id;
        std::uint64_t revision=0;
        std::string sourceWorldKey;
        std::map<Id,Entity> entities;
        std::map<Id,Segment> segments;
        std::map<Id,Profile> profiles;
        std::map<Id,Catalog> catalogs;
        std::map<Id,SharedTopology> topology;
    };
    struct TypeInfo {
        std::uint32_t version=1;
        bool compilerSupported=false;
    };
    using Registry=std::map<std::string,TypeInfo>;
    NEXUSV8RUNTIME_API Registry defaultRegistry();
    NEXUSV8RUNTIME_API Issues validate(const Document&, bool forCompilation=false, const Registry& = defaultRegistry());
    NEXUSV8RUNTIME_API Issues addEntity(Document&, Entity);
    NEXUSV8RUNTIME_API Issues renameEntity(Document&, const Id&, const std::string&);
    NEXUSV8RUNTIME_API Issues reparentEntity(Document&, const Id&, const Id&);
    NEXUSV8RUNTIME_API std::vector<Id> descendants(const Document&, const Id&);
    NEXUSV8RUNTIME_API Issues eraseEntity(Document&, const Id&, bool includeChildren=false);
    NEXUSV8RUNTIME_API std::set<Id> affectedHighways(const Document&, const Id& changedScope);
    struct ResolvedMesh {
        Id scope,catalog;
        std::string slot;
        MeshEntry entry;
        Issues issues;
        bool resolved=false,disabled=false;
    };
    using AssetProbe=std::function<bool(const std::string&)>;
    NEXUSV8RUNTIME_API ResolvedMesh resolveMesh(const Document&,const Id& highway,const std::string& slot,bool required,const AssetProbe&);
    NEXUSV8RUNTIME_API ResolvedMesh resolveMeshVariant(const Document&,const Id& highway,const std::string& exactSlot,const std::string& genericSlot,bool required,const AssetProbe&);
    NEXUSV8RUNTIME_API ResolvedMesh resolveTopologyMesh(const Document&,const Id& topology,const std::string& slot,bool required,const AssetProbe&);
    NEXUSV8RUNTIME_API Issues compatible(const MeshInterface&,const Profile&,const Profile&,double toleranceM=0.005);
    NEXUSV8RUNTIME_API const Profile* profileAt(const Document&,const Segment&,double stationM);
    NEXUSV8RUNTIME_API std::vector<LaneLink> proposeLinks(const Profile&,const Profile&,const std::function<Id()>& newId);
    NEXUSV8RUNTIME_API Issues validateAdapter(const Adapter&,const Profile&,const Profile&,double sourceLengthM);
    // Source scans are transient. Positions, signed tangents and frames never enter Document.
    struct SourcePoint {
        std::string key,scope,fingerprint;
        Availability availability=Availability::Loaded;
        Pose frame;
    };
    struct SourceEnd {
        std::string pointKey;
        double signedTangentM=0;
        Pose frame;
        Vec3 tangent;
    };
    struct Sample {
        double stationM=0;
        Vec3 position,forward{
            1,0,0
        },up{
            0,0,1
        };
    };
    struct SourceSegment {
        std::string key,scope,fingerprint;
        Availability availability=Availability::Loaded;
        SourceEnd start,end;
        std::vector<Sample> samples;
    };
    struct SourceScan {
        std::string worldKey, fingerprint;
        std::map<std::string,SourcePoint> points;
        std::map<std::string,SourceSegment> segments;
        std::set<std::string> completeScopes;
        // only scopes known fully loaded/enumerated
        std::set<std::string> deletedKeys;
        // explicit deletion evidence, never absence alone
    };
    enum class ChangeKind : std::uint8_t {
        Added, Changed, Unavailable, Deleted
    };
    struct ScanChange {
        ChangeKind kind;
        bool isSegment=false;
        Id entity;
        std::string sourceKey;
        Availability availability;
    };
    struct ScanDiff {
        std::uint64_t baseRevision=0;
        std::string scanFingerprint,documentFingerprint;
        Id highway;
        std::vector<ScanChange> changes;
        Issues issues;
    };
    NEXUSV8RUNTIME_API ScanDiff diffScan(const Document&,const SourceScan&,const Id& highway);
    NEXUSV8RUNTIME_API Issues applyScan(Document&,const SourceScan&,const ScanDiff&,const std::function<Id()>& newId);
    NEXUSV8RUNTIME_API Issues rebindSource(Document&,const Id&,const std::string& expectedOldKey,const std::string& newKey,const SourceScan&);
    // Full-document values make draft conflicts and replay explicit. Unreal uses its native
    // transaction buffer; this small session is the portable equivalent used by the CLI/tests.
    class NEXUSV8RUNTIME_API Session {
        Document committed_,draft_;
        std::vector<Document> undo_,redo_;
        std::uint64_t epoch_=0,baseEpoch_=0;
        public: explicit Session(Document initial);
        const Document& committed() const {
            return committed_;
        }
        Document& draft() {
            return draft_;
        }
        void revert();
        Issues apply();
        bool undo();
        bool redo();
        bool dirty() const;
        void replaceExternal(Document value);
    };
    using Bytes=std::vector<std::uint8_t>;
    NEXUSV8RUNTIME_API Bytes encode(const Document&);
    NEXUSV8RUNTIME_API Issues decode(const Bytes&,Document& out);
    NEXUSV8RUNTIME_API std::string contentFingerprint(const Document&);
    struct SaveResult {
        Issues issues;
        std::string hash;
        bool saved=false;
    };
    enum class SaveFault {
        None, BeforeWrite, BeforePromote
    };
    NEXUSV8RUNTIME_API SaveResult saveAtomic(const std::string& path,const Document&,const std::optional<std::string>& expectedHash,SaveFault=SaveFault::None);
    NEXUSV8RUNTIME_API Issues loadFile(const std::string&,Document& out);
    NEXUSV8RUNTIME_API SaveResult saveDraftAtomic(const std::string&,const Document& base,const Document& draft,const std::optional<std::string>& expectedHash);
    NEXUSV8RUNTIME_API Issues recoverDraft(const std::string&,const Document& current,Document& draftOut);
    NEXUSV8RUNTIME_API std::optional<std::string> fileFingerprint(const std::string&);
    struct LaneSample {
        double stationM=0,widthM=0;
        Vec3 center,left,right;
    };
    struct Corridor {
        Id id,segment,fromLane,toLane;
        Direction direction=Direction::Forward;
        std::uint32_t access=0;
        std::vector<LaneSample> samples;
    };
    struct Vertex {
        Vec3 position,normal;
        double u=0,v=0;
    };
    struct Surface {
        std::string role;
        std::vector<Vertex> vertices;
        std::vector<std::uint32_t> indices;
    };
    struct RoadOutput {
        Id segment;
        std::vector<Surface> surfaces;
        std::vector<Corridor> corridors;
        std::uint32_t textureWidth=256,textureHeight=512;
        Bytes markingRGBA;
    };
    struct PreviewSnapshot {
        std::uint32_t schema=SchemaVersion;
        Id document;
        std::uint64_t revision=0;
        std::string documentFingerprint,sourceFingerprint,contentHash;
        std::vector<RoadOutput> roads;
        std::map<Id,std::map<std::string,ResolvedMesh>> meshes;
        bool productionPublishable=false;
        // phases 1/2 are preview only
    };
    struct CompileOptions {
        double sampleSpacingM=1.0;
        double vehicleWidthM=1.8,vehicleLengthM=4.5,marginM=0.05;
        double maxGrade=0.35,maxBankRadians=0.8;
        bool requireAssets=true;
        std::function<bool()> canceled;
    };
    struct CompileResult {
        PreviewSnapshot snapshot;
        Issues issues;
        bool success=false;
    };
    NEXUSV8RUNTIME_API CompileResult compilePreview(const Document&,const SourceScan&,const AssetProbe&,const CompileOptions& = {
    });
    NEXUSV8RUNTIME_API Issues checkContainment(const Corridor&,double widthM,double lengthM,double marginM);
    NEXUSV8RUNTIME_API Sample sampleSource(const SourceSegment&,double stationM);
    NEXUSV8RUNTIME_API std::vector<SourceEnd> connectedEnds(const SourceScan&,const std::string& pointKey);
    NEXUSV8RUNTIME_API Document makeExample(const std::function<Id()>& newId);
    NEXUSV8RUNTIME_API SourceScan makeExampleScan();
}
// namespace nexus
