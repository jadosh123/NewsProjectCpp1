#include "DrawThread.h"
#include "GuiMain.h"
#include "../../shared/ImGuiSrc/imgui.h"
#include <iostream>
#include <thread>
#include "DownloadThread.h"
#include "SearchThread.h"
#include "Favorite.h"

std::atomic<bool> onHomePage = true;

//creates a thread for searching keyword
void SearchKeyword(CommonObjects* common, char* buff) {

	SearchThread search;
	auto search_th = std::jthread([&] {search(*common); });
	search.SetUrl(buff);
	std::cout << "running...\n";

	search_th.join();
}


//creates a thread for showing homepage
void showTrending(CommonObjects* common) {
	DownloadThread down;
	auto down_th = std::jthread([&] {down(*common); });

	down_th.join();
}

//Draws the graphics for the app
void DrawAppWindow(void* common_ptr)
{
	auto common = (CommonObjects*)common_ptr;
	ImGui::Begin("Connected!");
	ImGui::Text("Search for the news");
	static char buff[200];
	ImGui::InputText("News", buff, sizeof(buff));
	ImGui::SameLine();

	//searches for keyword input by user
	if (ImGui::Button("search")) {

		common->userSearch = buff;
		common->user_search = true;
		onHomePage = false;

		SearchKeyword(common, buff);
	}
	ImGui::SameLine();
	//shows favorited articles
	if (ImGui::Button("Show Favorites")) {
		common->display_favorites = true;
	}

	//Returns to home page if not on it
	ImGui::SameLine();
	if (ImGui::Button("Home Page")) {
		if (!onHomePage) {
			showTrending(common);
		}
	}

	//goes to favorites page
	if (common->display_favorites) {
		ImGui::Begin("Favorite Articles");
		ImGui::SetWindowSize(ImVec2(1100.0f, 600.0f));

		//button to close the favorites tab
		if (ImGui::Button("Close Favorites")) {
			common->display_favorites = false;
		}

		if (ImGui::BeginTable("Favorite Articles", 3)) {
			ImGui::TableSetupColumn("Name");
			ImGui::TableSetupColumn("Title");
			ImGui::TableSetupColumn("PublishedAt");
			ImGui::TableHeadersRow();

			for (auto& a : common->favorite_articles) {
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text(a.author.c_str());
				ImGui::TableSetColumnIndex(1);
				ImGui::Selectable(a.title.c_str());

				//checks if the news article was clicked
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
					a.display_content = true;
				}


				//a flag that indicates to open the window and display the content
				if (a.display_content) {
					ImGui::Begin(a.author.c_str());
					ImGui::SetWindowSize(ImVec2(800.0f, 400.0f));

					if (ImGui::Button("Close")) {
						a.display_content = false;
					}

					ImGui::SameLine();
					if (ImGui::Button("Unfavorite")) {
						common->removeArticle(*common, a.title);
						a.display_content = false;
					}

					ImGui::Text(a.title.c_str());
					ImGui::Spacing();
					ImGui::Spacing();


					ImGui::TextWrapped("%s", a.description.c_str());
					ImGui::Spacing();
					ImGui::Spacing();


					ImGui::TextWrapped("%s", a.content.c_str());
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Spacing();

					ImGui::TextWrapped("Url to full article:\n%s", a.url.c_str());
					
					if (ImGui::Button("Copy to clipboard")) {
						ImGui::SetClipboardText(a.url.c_str());
					}


					ImGui::End();
				}
				ImGui::TableSetColumnIndex(2);
				ImGui::Text(a.publishedAt.c_str());

			}
			ImGui::EndTable();
			
		}

		ImGui::End();
	}



	//Checks if data is ready to display
	if (common->data_ready)
	{
		if (ImGui::BeginTable("Articles", 3))
		{
			ImGui::TableSetupColumn("Name");
			ImGui::TableSetupColumn("Title");
			ImGui::TableSetupColumn("PublishedAt");
			ImGui::TableHeadersRow();
			

			for (auto& a : common->articles) {
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text(a.author.c_str());
				ImGui::TableSetColumnIndex(1);
				ImGui::Selectable(a.title.c_str());

				//checks if the news article was clicked
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
					a.display_content = true;
				}

				//a flag that indicates to open the window and display the content
				if (a.display_content) {
					ImGui::Begin(a.author.c_str());
					ImGui::SetWindowSize(ImVec2(800.0f, 400.0f));

					if (ImGui::Button("Close")) {
						a.display_content = false;
					}
					ImGui::SameLine();
					if (ImGui::Button("Favorite")) {
						common->copyArticle(*common, a.title);
					}

					ImGui::Text(a.title.c_str());
					ImGui::Spacing();
					ImGui::Spacing();


					ImGui::TextWrapped("%s", a.description.c_str());
					ImGui::Spacing();
					ImGui::Spacing();


					ImGui::TextWrapped("%s", a.content.c_str());
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Spacing();

					ImGui::TextWrapped("Url to full article:\n%s", a.url.c_str());

					if (ImGui::Button("Copy to clipboard")) {
						ImGui::SetClipboardText(a.url.c_str());
					}

					ImGui::End();
				}
				
				ImGui::TableSetColumnIndex(2);
				ImGui::Text(a.publishedAt.c_str());
				
			}
			
			ImGui::EndTable();
		}
	}
	ImGui::End();
}

void DrawThread::operator()(CommonObjects& common)
{
	GuiMain(DrawAppWindow, &common);
}


