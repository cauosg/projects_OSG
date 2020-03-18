//handrock을 패턴 종료시 파괴하는 스크립트입니다

using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class destroymanage : MonoBehaviour {

    public GameObject handrock;
	// Use this for initialization
	void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {
        if (handrock.transform.position.y < -100)
            Destroy(gameObject);
	}
}
