using System;
using UnityEngine;

public class controlMovement : MonoBehaviour
{
    public float speed = 5f;
    public float angularSpeed = 100f;

    private float vertAngle = 0f;
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        float horizontalAxisInput = Input.GetAxis("Horizontal");
        float verticalAxisInput = Input.GetAxis("Vertical");

        transform.Translate(horizontalAxisInput * speed * Time.deltaTime, 0, verticalAxisInput * speed * Time.deltaTime);

        float mouseInputX = Input.GetAxis("Mouse X");
        float mouseInputY = Input.GetAxis("Mouse Y");

        vertAngle -= mouseInputX * angularSpeed * Time.deltaTime;
        vertAngle -= Mathf.Clamp(vertAngle, 0f, 90f);

        transform.rotation = Quaternion.Euler(vertAngle,transform.eulerAngles.x+ mouseInputY *angularSpeed * Time.deltaTime,0);
    }
}
