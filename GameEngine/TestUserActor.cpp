#ifndef TEST_USER_ACTOR_H
#define TEST_USER_ACTOR_H

#include "Engine.h"
#include "Level.h"
#include "CoreLib/LibUI/LibUI.h"

using namespace GraphicsUI;
using namespace GameEngine;

class TestUserActor : public Actor
{
private:
	RefPtr<Drawable> boxDrawable;
	Material material;
	Form * uiForm = nullptr;
public:
	virtual EngineActorType GetEngineType() override
	{
		return EngineActorType::Drawable;
	}
	virtual void RegisterUI(GraphicsUI::UIEntry * entry) override
	{
		uiForm = new Form(entry);
		uiForm->SetText("Color Control");
		auto btnRed = new Button(uiForm);
		btnRed->SetText("Red");
		btnRed->Posit(EM(1.0f), EM(1.5f), EM(3.0f), EM(1.5f));
		btnRed->OnClick.Bind([this](UI_Base*)
		{
			material.SetVariable("solidColor", DynamicVariable(Vec3::Create(1.0f, 0.0f, 0.0f)));
		});

		auto btnBlue = new Button(uiForm);
		btnBlue->SetText("Blue");
		btnBlue->Posit(EM(4.5f), EM(1.5f), EM(3.0f), EM(1.5f));
		btnBlue->OnClick.Bind([this](UI_Base*)
		{
			material.SetVariable("solidColor", DynamicVariable(Vec3::Create(0.0f, 0.0f, 1.0f)));
		});

		auto slider = new ScrollBar(uiForm);
		slider->Posit(EM(1.0f), EM(3.5f), EM(6.5f), EM(1.0f));
		slider->SetValue(0, 2000, 1000, 50);
		slider->OnChanged.Bind([=](UI_Base*)
		{
			localTransform.values[12] = (slider->GetPosition() - 1000) * 0.2f;
		});
        auto container = new GraphicsUI::ScrollPanel(uiForm);
        container->Posit(10, EM(5.0), EM(40.0f), EM(35.0f));
        auto updateContainer = [=](float zoom)
        {
            container->ClearChildren();
            for (int i = 0; i < 1000; i++)
                for (int j = 0; j < 100; j++)
                {
                    auto bezier = new BezierCurve(container);
                    bezier->SetPoints(Math::Max(1.0f, 5.0f * zoom), Vec2::Create(30.0f + i * 100, 200.0f + j * 60)*zoom, Vec2::Create(40.0f + i * 100, 120.0f + j * 60)*zoom, Vec2::Create(240.0f + i * 100, 280.0f + j * 60)*zoom, Vec2::Create(250.0f + i * 100, 200.0f + j * 60)*zoom);
                    bezier->StartCap = bezier->EndCap = LineCap::Arrow;
                }
            for (int i = 0; i < 1000; i++)
                for (int j = 0; j < 100; j++)
                {
                    auto circle = new GraphicsUI::Ellipse(container);
                    circle->FontColor = Color(255, 255, 255, 255);
                    circle->Posit((int)((140 + i * 140)*zoom), (int)((140 + j * 140)*zoom), (int)(60 * zoom), (int)(60 * zoom));
                    circle->OnMouseEnter.Bind([=](UI_Base*) 
                    {
                        circle->FontColor = Color(255, 255, 0, 255); 
                    });
                    circle->OnMouseLeave.Bind([=](UI_Base*) {circle->FontColor = Color(255, 255, 255, 255); });

                }
            container->UpdateLayout();
        };
        updateContainer(1.0f);
        container->EnableZoom = true;
        container->OnZoom.Bind([=](float /*zoom0*/, float zoom1) {updateContainer(zoom1); });
        entry->ShowWindow(uiForm);
	}
	virtual void OnLoad() override
	{
		Actor::OnLoad();
		material = *Engine::Instance()->GetLevel()->LoadMaterial("SolidColor.material");
	}
	virtual void OnUnload() override
	{
		Actor::OnUnload();
		if (uiForm)
			Engine::Instance()->GetUiEntry()->RemoveForm(uiForm);
	}
	virtual void GetDrawables(const GetDrawablesParameter & param) override
	{
		if (!boxDrawable)
		{
			Mesh boxMesh = Mesh::CreateBox(VectorMath::Vec3::Create(-30.0f, 0.0f, -30.0f), VectorMath::Vec3::Create(30.0, 80.0f, 30.0f));
			boxDrawable = param.rendererService->CreateStaticDrawable(&boxMesh, &material);
		}
		if (boxDrawable)
		{
			boxDrawable->UpdateTransformUniform(localTransform);
			param.sink->AddDrawable(boxDrawable.Ptr());
		}
	}
};

void RegisterTestUserActor()
{
	Engine::Instance()->RegisterActorClass("TestUser", []() {return new TestUserActor(); });
}

#endif