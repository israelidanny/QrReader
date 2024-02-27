#pragma once

#include "CoreMinimal.h"
#include "MediaTexture.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "QrReaderBlueprintFunctionLibrary.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FQrScanCallback, const TArray<UZXingResult*>&, Result);

/**
 * 
 */
UCLASS()
class QRREADER_API UQrReaderBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "QR Reader", meta = (AutoCreateRefTerm = "OnResult"))
	static void QrScanFromTexture(UTexture2D* Texture, const FQrScanCallback& OnResult);

	UFUNCTION(BlueprintCallable, Category = "QR Reader", meta = (AutoCreateRefTerm = "OnResult"))
	static void QrScanFromMediaTexture(UMediaTexture* MediaTexture, const FQrScanCallback& OnResult);
};
