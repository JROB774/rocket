#include "cs_assets.hpp"
#include "cs_platform.hpp"

#include <algorithm>
#include <fstream>

using namespace cs;

CS_PUBLIC_SCOPE
{
    AssetManager s_assetManager;

    CS_API void InitAssetManager()
    {
        // The executable's location is the highest priority location for assets.
        s_assetManager.assetPaths.push_back(GetExecPath());

        // If there's other asset locations add them with descending priority.
        std::ifstream file("asset_paths.txt", std::ios::in);
        if(file.is_open())
        {
            std::string path;
            while(std::getline(file, path))
            {
                s_assetManager.assetPaths.push_back(GetExecPath() + ValidatePath(path));
            }
        }
    }

    CS_API void QuitAssetManager()
    {
        for(auto& [name,asset]: s_assetManager.assetMap)
        {
            if(asset)
            {
                asset->Free();
                Deallocate(asset);
            }
        }
        s_assetManager.assetMap.clear();
    }

    CS_API void AssetManagerDebugUi(bool& open)
    {
        enum ColumnID
        {
            ColumnID_Type,
            ColumnID_Name,
            ColumnID_Load,
            ColumnID_File,
            ColumnID_TOTAL
        };

        static const float k_numRowsVisible = 20.0f;
        static size_t s_lastAssetListSize = 0;
        static std::string s_selectedAsset = "";
        static ImGuiTextFilter s_assetFilter;

        // Table listing all of the assets currently cached and their metadata.
        ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable|ImGuiTableFlags_Sortable|ImGuiTableFlags_ScrollX|ImGuiTableFlags_ScrollY|ImGuiTableFlags_BordersOuter|ImGuiTableFlags_RowBg;
        if(ImGui::BeginTable("Assets", ColumnID_TOTAL, tableFlags, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * k_numRowsVisible)))
        {
            ImGui::TableSetupScrollFreeze(0, 1); // Make header row always visible.
            ImGui::TableSetupColumn("Type",   ImGuiTableColumnFlags_WidthFixed, 0.0f, ColumnID_Type);
            ImGui::TableSetupColumn("Name",   ImGuiTableColumnFlags_WidthFixed, 0.0f, ColumnID_Name);
            ImGui::TableSetupColumn("Loaded", ImGuiTableColumnFlags_WidthFixed, 0.0f, ColumnID_Load);
            ImGui::TableSetupColumn("File",   ImGuiTableColumnFlags_WidthFixed, 0.0f, ColumnID_File);
            ImGui::TableHeadersRow();

            // Handle sorting the items.
            ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs();
            if(sortSpecs && s_assetManager.assetList.size() > 1)
            {
                if(sortSpecs->SpecsDirty || s_assetManager.assetList.size() != s_lastAssetListSize)
                {
                    std::sort(s_assetManager.assetList.begin(), s_assetManager.assetList.end(),
                    [=](const AssetBase* a, const AssetBase* b)
                    {
                        bool result = false;
                        for(int i=0; i<sortSpecs->SpecsCount; ++i)
                        {
                            const ImGuiTableColumnSortSpecs* sortSpec = &sortSpecs->Specs[i];
                            switch(sortSpec->ColumnUserID)
                            {
                                case(ColumnID_Type): result = a->GetType() < b->GetType(); break;
                                case(ColumnID_Name): result = a->m_name < b->m_name; break;
                                case(ColumnID_Load): result = a->m_loaded < b->m_loaded; break;
                                case(ColumnID_File): result = a->m_fileName < b->m_fileName; break;
                                default:
                                {
                                    CS_ASSERT(false, "Unknown column ID for sorting!");
                                } break;
                            }
                            return (sortSpec->SortDirection == ImGuiSortDirection_Ascending) ? result : !result;
                        }

                        // Fallback to sorting by type.
                        return a->GetType() < b->GetType();
                    });
                    sortSpecs->SpecsDirty = false;
                }
            }

            for(auto& asset: s_assetManager.assetList)
            {
                if(s_assetManager.assetFilters[asset->GetType()] && s_assetFilter.PassFilter(asset->m_fileName.c_str()))
                {
                    ImGui::TableNextRow();

                    bool selected = (s_selectedAsset == asset->m_lookup);
                    if(selected)
                    {
                        ImU32 rowColor = ImGui::GetColorU32(ImGuiCol_CheckMark);
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, rowColor);
                    }
                    else if(!asset->m_loaded)
                    {
                        ImU32 rowColor = ImGui::GetColorU32(Vec4(1,0,0,0.65f));
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, rowColor);
                    }

                    ImGui::PushID(asset->m_lookup.c_str());
                    ImGui::TableNextColumn();
                    ImGuiSelectableFlags selectableFlags = ImGuiSelectableFlags_SpanAllColumns|ImGuiSelectableFlags_AllowItemOverlap;
                    if(ImGui::Selectable(asset->GetType(), false, selectableFlags))
                    {
                        if(!selected)
                            s_selectedAsset = asset->m_lookup;
                        else
                            s_selectedAsset.clear();
                    }
                    ImGui::TableNextColumn(); ImGui::Text(asset->m_name.c_str());
                    ImGui::TableNextColumn(); ImGui::Text(asset->m_loaded ? "True" : "False");
                    ImGui::TableNextColumn(); ImGui::Text(asset->m_fileName.c_str());
                    ImGui::PopID();
                }
            }

            ImGui::EndTable();

            s_lastAssetListSize = s_assetManager.assetList.size();
        }

        // Input for filtering the assets based on their file name.
        ImGui::Text("Filter:");
        ImGui::SameLine();
        s_assetFilter.Draw("##filter", -1.0f);

        // Buttons for filtering different types of assets.
        f32 itemCount = CS_CAST(f32, s_assetManager.assetFilters.size());
        f32 regionWidth = ImGui::GetWindowContentRegionWidth() - (ImGui::GetStyle().ItemSpacing.x * (itemCount-1));
        f32 buttonWidth = regionWidth / itemCount;
        size_t filterIndex = 0;
        for(auto& [type,active]: s_assetManager.assetFilters)
        {
            ImGui::ToggleButton(type.c_str(), &active, NULL, NULL, ImVec2(buttonWidth,0));
            if(filterIndex++ < s_assetManager.assetFilters.size()-1) ImGui::SameLine();
        }

        // If an asset in the above table is selected then show its custom debug view.
        if(!s_selectedAsset.empty())
        {
            AssetBase* asset = s_assetManager.assetMap[s_selectedAsset];
            if(asset && asset->m_loaded)
            {
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

                if(ImGui::Button("Reload Asset", ImVec2(-1,0)))
                {
                    if(asset->m_loaded) asset->Free();
                    asset->Load(asset->m_fileName);
                }
                if(ImGui::CollapsingHeader("Asset View", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    asset->DoDebugView();
                }
            }
        }
    }
}
